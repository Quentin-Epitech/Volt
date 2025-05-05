#include "../screen/screen.h"

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

bool Screen::ValidateCoordinates(const ScreenPoint& p1, const ScreenPoint& p2)
{
    if (p1.x < 0 || p1.y < 0 || p2.x < 0 || p2.y < 0)
    {
        std::cerr << Logger::Error() << "Coordinates cannot be negative." << std::endl;
        return false;
    }
    return true;
}
#endif