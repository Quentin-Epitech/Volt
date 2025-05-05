// src/base/screen/screen.h
#pragma once

#include <string>
#include <fstream>
#include "../common_types.h"  // Inclure les types communs
#include "../utils/logger.h"

class Screen
{
public:
    static void Init(const ScreenPoint& p1, const ScreenPoint& p2);
    static void Loop();

private:
    static inline const std::string m_PngPath = "image.png";
    inline static ScreenPoint m_StartPoint;
    inline static ScreenPoint m_EndPoint;

    static void SaveScreenshot();
    static bool ValidateCoordinates(const ScreenPoint& p1, const ScreenPoint& p2);
};