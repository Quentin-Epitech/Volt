// Assurez-vous d'inclure common_types.h en premier
#include "../common_types.h"
#include "screen.h"
#include <algorithm>  // Pour std::min et std::max
#include <fstream>    // Pour la manipulation de fichiers

#ifdef MACOS
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>
#include <CoreFoundation/CoreFoundation.h>

void Screen::Init(const ScreenPoint& p1, const ScreenPoint& p2)
{
    m_StartPoint = p1;
    m_EndPoint = p2;
    SaveScreenshot();
}

void Screen::Loop()
{
    SaveScreenshot();
}

void Screen::SaveScreenshot()
{
    if (!ValidateCoordinates(m_StartPoint, m_EndPoint)) return;

    int left = std::min(m_StartPoint.x, m_EndPoint.x);
    int top = std::min(m_StartPoint.y, m_EndPoint.y);
    int width = std::max(m_StartPoint.x, m_EndPoint.x) - left;
    int height = std::max(m_StartPoint.y, m_EndPoint.y) - top;

    // Capturer la partie de l'écran désignée
    CGDirectDisplayID displayID = CGMainDisplayID();
    
    // Sur macOS, les coordonnées Y sont inversées par rapport à Windows
    // L'origine est en bas à gauche au lieu d'en haut à gauche
    CGRect displayBounds = CGDisplayBounds(displayID);
    int macTop = displayBounds.size.height - (top + height);
    
    CGImageRef screenshot = CGDisplayCreateImageForRect(
        displayID, 
        CGRectMake(left, macTop, width, height)
    );
    
    if (!screenshot) {
        std::cerr << Logger::Error() << "Failed to capture screen area." << std::endl;
        return;
    }
    
    // Convertir et sauvegarder l'image en PNG
    CFURLRef url = CFURLCreateWithFileSystemPath(
        kCFAllocatorDefault,
        CFStringCreateWithCString(kCFAllocatorDefault, m_PngPath.c_str(), kCFStringEncodingUTF8),
        kCFURLPOSIXPathStyle,
        false
    );
    
    if (!url) {
        std::cerr << Logger::Error() << "Failed to create URL for saving image." << std::endl;
        CFRelease(screenshot);
        return;
    }
    
    // Format UTI pour PNG
    CFStringRef type = CFSTR("public.png");
    
    // Créer un contexte de destination pour l'image
    CGImageDestinationRef destination = CGImageDestinationCreateWithURL(
        url,
        type,
        1,
        nullptr
    );
    
    if (!destination) {
        std::cerr << Logger::Error() << "Failed to create image destination." << std::endl;
        CFRelease(url);
        CFRelease(screenshot);
        return;
    }
    
    // Ajouter l'image au contexte et finaliser
    CGImageDestinationAddImage(destination, screenshot, nullptr);
    bool success = CGImageDestinationFinalize(destination);
    
    // Libérer les ressources
    CFRelease(destination);
    CFRelease(url);
    CFRelease(screenshot);
    
    if (!success) {
        std::cerr << Logger::Error() << "Failed to save image to file." << std::endl;
    }
}

#elif defined(WINDOWS)
#include <windows.h>

void Screen::Init(const ScreenPoint& p1, const ScreenPoint& p2)
{
    m_StartPoint = p1;
    m_EndPoint = p2;
    SaveScreenshot();
}

void Screen::Loop()
{
    SaveScreenshot();
}

void Screen::SaveScreenshot()
{
    if (!ValidateCoordinates(m_StartPoint, m_EndPoint)) return;

    int left = std::min(m_StartPoint.x, m_EndPoint.x);
    int top = std::min(m_StartPoint.y, m_EndPoint.y);
    int width = std::max(m_StartPoint.x, m_EndPoint.x) - left;
    int height = std::max(m_StartPoint.y, m_EndPoint.y) - top;

    HDC hScreenDC = GetDC(NULL);
    if (!hScreenDC)
    {
        std::cerr << Logger::Error() << "Failed to get screen DC." << std::endl;
        return;
    }

    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    if (!hMemoryDC)
    {
        std::cerr << Logger::Error() << "Failed to create compatible DC." << std::endl;
        ReleaseDC(NULL, hScreenDC);
        return;
    }

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    if (!hBitmap)
    {
        std::cerr << Logger::Error() << "Failed to create compatible bitmap." << std::endl;
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return;
    }

    SelectObject(hMemoryDC, hBitmap);
    if (!BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, left, top, SRCCOPY))
    {
        std::cerr << Logger::Error() << "Failed to capture screen area." << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return;
    }

    BITMAP bitmap;
    if (!GetObject(hBitmap, sizeof(BITMAP), &bitmap))
    {
        std::cerr << Logger::Error() << "Failed to retrieve bitmap object." << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return;
    }

    int rowSize = ((width * 3 + 3) & ~3);
    int imageSize = rowSize * height;

    BITMAPFILEHEADER bfHeader = {};
    bfHeader.bfType = 0x4D42;
    bfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + imageSize;
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER biHeader = {};
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biWidth = width;
    biHeader.biHeight = -height;
    biHeader.biPlanes = 1;
    biHeader.biBitCount = 24;
    biHeader.biCompression = BI_RGB;

    char* pixels = new (std::nothrow) char[imageSize]();
    if (!pixels)
    {
        std::cerr << Logger::Error() << "Memory allocation for pixel data failed." << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return;
    }

    if (!GetDIBits(hMemoryDC, hBitmap, 0, height, pixels, reinterpret_cast<BITMAPINFO*>(&biHeader), DIB_RGB_COLORS))
    {
        std::cerr << Logger::Error() << "Failed to retrieve bitmap data." << std::endl;
        delete[] pixels;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return;
    }

    std::ofstream outFile(m_PngPath, std::ios::binary);
    if (!outFile.is_open())
    {
        std::cerr << Logger::Error() << "Failed to open output file." << std::endl;
        delete[] pixels;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return;
    }

    outFile.write(reinterpret_cast<char*>(&bfHeader), sizeof(bfHeader));
    outFile.write(reinterpret_cast<char*>(&biHeader), sizeof(biHeader));
    if (!outFile.write(pixels, imageSize))
    {
        std::cerr << Logger::Error() << "Failed to write pixel data to file." << std::endl;
    }

    delete[] pixels;
    outFile.close();
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);
}
#endif

// Fonction commune indépendante de la plateforme
bool Screen::ValidateCoordinates(const ScreenPoint& p1, const ScreenPoint& p2)
{
    if (p1.x < 0 || p1.y < 0 || p2.x < 0 || p2.y < 0)
    {
        std::cerr << Logger::Error() << "Coordinates cannot be negative." << std::endl;
        return false;
    }
    return true;
}