// src/base/common_types.h
#pragma once

#ifdef WINDOWS
    #include <windows.h>
    typedef POINT ScreenPoint;
#elif defined(MACOS)
    struct ScreenPoint {
        int x;
        int y;
    };
#endif