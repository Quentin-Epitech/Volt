#include "../platform/platform.h"

#ifdef WINDOWS
#include <windows.h>
#include <thread>
#include <chrono>

void Platform::ClearConsole()
{
    system("cls");
}

void Platform::SetConsoleEncoding()
{
    SetConsoleOutputCP(CP_UTF8);
    std::locale::global(std::locale("en_US.UTF-8"));
    std::wcout.imbue(std::locale());
}

void Platform::SetConsoleColor(int textColor, int bgColor)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

bool Platform::GetKeyState(int keyCode)
{
    return (GetAsyncKeyState(keyCode) & 0x8000) != 0;
}

ScreenPoint Platform::GetCursorPosition()
{
    POINT point;
    GetCursorPos(&point);
    return point;
}

void Platform::WaitForKeyPress(int keyCode)
{
    while (!(GetAsyncKeyState(keyCode) & 0x8000));
    while (GetAsyncKeyState(keyCode) & 0x8000);
}

void Platform::Sleep(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

#endif