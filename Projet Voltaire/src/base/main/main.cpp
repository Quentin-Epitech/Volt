#include "main.h"

void Main::Init()
{
    Platform::SetConsoleEncoding();
    
    // Sur Windows, cette fonction active la conscience de la mise à l'échelle DPI
    #ifdef WINDOWS
    SetProcessDPIAware();
    #endif

    std::cout << Logger::Info() << "Press Enter when you are ready" << std::endl;
    
    // Attente de la touche Entrée
    while (!Platform::GetKeyState(Platform::KEY_ENTER));
    Platform::Sleep(100);

    Base::Init();
}

void Main::Loop()
{
    while (true)
    {
        Platform::Sleep(500);
        Base::Loop();
    }
}

int main()
{
    Main::Init();
    Main::Loop();
}