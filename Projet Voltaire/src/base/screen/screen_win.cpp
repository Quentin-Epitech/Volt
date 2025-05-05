#include "../screen/screen.h"

#ifdef WINDOWS
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

    int left = min(m_StartPoint.x, m_EndPoint.x);
    int top = min(m_StartPoint.y, m_EndPoint.y);
    int width = max(m_StartPoint.x, m_EndPoint.x) - left;
    int height = max(m_StartPoint.y, m_EndPoint.y) - top;

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