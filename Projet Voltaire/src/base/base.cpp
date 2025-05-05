#include "base.h"

void Base::Init()
{
    Extract::Init();

    CaptureClickCoordinates();
    NormalizeCoordinates();

    Screen::Init(m_StartPoint, m_EndPoint);

    std::cout << Logger::Info() << "Waiting for text!" << std::endl;
}

void Base::Loop()
{    
    Screen::Loop();

    const std::string newInput = SanitizeString(Extract::ExtractText());

    if (newInput == m_CurrentText || newInput.empty()) return;

    m_CurrentText = newInput;

    Platform::ClearConsole();
    
    std::cout << Logger::Info() << "Input: " << m_CurrentText << std::endl;

    Reverso::Loop();
    LanguageTool::Loop();
}

std::string Base::GetCurrentText()
{
    return m_CurrentText;
}

std::string Base::SanitizeString(const std::string& input)
{
    std::string sanitized = input;

    for (char& c : sanitized)
    {
        if (c == '\n') c = ' ';
    }

    size_t pos;
    while ((pos = sanitized.find("  ")) != std::string::npos)
    {
        sanitized.replace(pos, 2, " ");
    }

    if (!sanitized.empty() && sanitized[sanitized.size() - 1] == ' ')
    {
        sanitized.pop_back();
    }
    return sanitized;
}

void Base::CaptureClickCoordinates()
{
    std::cout << Logger::Info() << "Waiting for first click..." << std::endl;

    // Attente du premier clic
    while (!Platform::GetKeyState(Platform::KEY_LBUTTON));
    m_StartPoint = Platform::GetCursorPosition();
    while (Platform::GetKeyState(Platform::KEY_LBUTTON));
    Platform::Sleep(100);

    std::cout << Logger::Info() << "Waiting for second click..." << std::endl;

    // Attente du second clic
    while (!Platform::GetKeyState(Platform::KEY_LBUTTON));
    m_EndPoint = Platform::GetCursorPosition();
    while (Platform::GetKeyState(Platform::KEY_LBUTTON));
}

void Base::NormalizeCoordinates()
{
    if (m_StartPoint.x > m_EndPoint.x || m_StartPoint.y > m_EndPoint.y)
    {
        std::swap(m_StartPoint, m_EndPoint);
    }
}