// src/base/platform/platform.h
#pragma once

#include <string>
#include <iostream>
#include "../common_types.h"  // Inclure les types communs

class Platform
{
public:
    // Méthodes console
    static void ClearConsole();
    static void SetConsoleEncoding();
    static void SetConsoleColor(int textColor, int bgColor);
    
    // Méthodes d'entrée
    static bool GetKeyState(int keyCode);
    static ScreenPoint GetCursorPosition();
    static void WaitForKeyPress(int keyCode);
    
    // Méthodes utilitaires
    static void Sleep(int milliseconds);
    
    // Constantes pour les codes de touche
    #ifdef WINDOWS
        static const int KEY_ENTER = VK_RETURN;
        static const int KEY_LBUTTON = VK_LBUTTON;
    #elif defined(MACOS)
        static const int KEY_ENTER = 36;  // Keycode pour Enter sur macOS
        static const int KEY_LBUTTON = 0;  // Code pour le clic gauche sur macOS
    #endif
};