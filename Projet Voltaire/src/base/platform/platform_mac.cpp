#include "../platform/platform.h"

#ifdef MACOS
#include <thread>
#include <chrono>
#include <CoreGraphics/CoreGraphics.h>
#include <AppKit/AppKit.h>

void Platform::ClearConsole()
{
    // Effacer la console avec des codes ANSI (fonctionne sur Terminal macOS)
    std::cout << "\033[2J\033[1;1H";
}

void Platform::SetConsoleEncoding()
{
    // UTF-8 est généralement le défaut sur macOS, mais on le force pour être sûr
    std::locale::global(std::locale("en_US.UTF-8"));
    std::wcout.imbue(std::locale());
}

void Platform::SetConsoleColor(int textColor, int bgColor)
{
    // Codes ANSI pour les couleurs de console
    std::string textColorCode;
    
    switch (textColor) {
        case 0: textColorCode = "30"; break; // noir
        case 1: textColorCode = "34"; break; // bleu
        case 2: textColorCode = "32"; break; // vert
        case 4: textColorCode = "31"; break; // rouge
        case 7: textColorCode = "37"; break; // blanc
        default: textColorCode = "37";       // blanc par défaut
    }
    
    std::string bgColorCode;
    
    switch (bgColor) {
        case 0: bgColorCode = "40"; break; // noir
        case 1: bgColorCode = "44"; break; // bleu
        case 2: bgColorCode = "42"; break; // vert
        case 4: bgColorCode = "41"; break; // rouge
        case 7: bgColorCode = "47"; break; // blanc
        default: bgColorCode = "40";       // noir par défaut
    }
    
    std::cout << "\033[" + textColorCode + ";" + bgColorCode + "m";
}

bool Platform::GetKeyState(int keyCode)
{
    // Pour macOS, on utilise le CGEventSource pour vérifier l'état des touches
    CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);
    bool keyDown = CGEventSourceKeyState(source, (CGKeyCode)keyCode);
    CFRelease(source);
    return keyDown;
}

ScreenPoint Platform::GetCursorPosition()
{
    // Obtenir la position du curseur sur macOS
    CGEventRef event = CGEventCreate(nullptr);
    CGPoint point = CGEventGetLocation(event);
    CFRelease(event);
    
    ScreenPoint result;
    result.x = static_cast<int>(point.x);
    result.y = static_cast<int>(point.y);
    return result;
}

void Platform::WaitForKeyPress(int keyCode)
{
    // Attente de l'appui sur une touche
    while (!GetKeyState(keyCode)) {
        Sleep(10);
    }
    
    // Attente du relâchement de la touche
    while (GetKeyState(keyCode)) {
        Sleep(10);
    }
}

void Platform::Sleep(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

#endif