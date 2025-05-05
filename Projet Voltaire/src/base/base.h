// src/base/base.h
#pragma once

#include "common_types.h"  // Inclure les types communs

// Ensuite les autres fichiers
#include "network/net.h"
#include "parsing/reverso/reverso.h"
#include "parsing/languageTool/languageTool.h"
#include "tesseract/extract.h"
#include "utils/encode.h"
#include "utils/logger.h"
#include "screen/screen.h"
#include "platform/platform.h"

class Base
{
public:
    static void Init();
    static void Loop();

    static std::string GetCurrentText();

private:
    static std::string SanitizeString(const std::string& input);
    static void CaptureClickCoordinates();
    static void NormalizeCoordinates();

    inline static ScreenPoint m_StartPoint;
    inline static ScreenPoint m_EndPoint;
    inline static std::string m_CurrentText;
};