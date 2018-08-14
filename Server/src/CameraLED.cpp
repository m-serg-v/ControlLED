#include "pch.h"

#include "CameraLED.h"

#define LED_RED "\e[1;31m"
#define LED_GREEN "\e[1;32m"
#define LED_BLUE "\e[1;34m"
#define LED_NC "\e[0m"

std::shared_ptr<CameraLED> CameraLED::CameraLED_impl = nullptr;

int CameraLED::MinRate = 0;
int CameraLED::MaxRate = 5;

std::shared_ptr<CameraLED> CameraLED::Get()
{
    if (!CameraLED_impl)
    {
        CameraLED_impl = std::make_shared<CameraLED>();
    }

    return CameraLED::CameraLED_impl;
}

CameraLED::CameraLED()
    :
    m_State(StateLED::OFF),
    m_Color(ColorLED::RED),
    m_Rate(CameraLED::MinRate)
{
}

CameraLED::~CameraLED()
{
}

int CameraLED::ClampRate(int value)
{
    if (value < CameraLED::MinRate)
        value = CameraLED::MinRate;
    if (value > CameraLED::MaxRate)
        value = CameraLED::MaxRate;

    return value;
}

StateLED CameraLED::GetState() const
{
    return m_State;
}

void CameraLED::SetState(StateLED state)
{
    m_State = state;
}

ColorLED CameraLED::GetColor() const
{
    return m_Color;
}

void CameraLED::SetColor(ColorLED color)
{
    m_Color = color;
}

int CameraLED::GetRate() const
{
    return m_Rate;
}

void CameraLED::SetRate(int rate)
{
    m_Rate = CameraLED::ClampRate(rate);
}

std::string CameraLED::GetStateStr(StateLED state)
{
    switch (state)
    {
    case StateLED::ON:
        return "ON";
    case StateLED::OFF:
        return "OFF";
    default:
        break;
    }

    return "UNKNOWN";
}

std::string CameraLED::GetColorCodeStr(ColorLED color)
{
    switch (color)
    {
    case ColorLED::RED:
        return LED_RED;
    case ColorLED::GREEN:
        return LED_GREEN;
    case ColorLED::BLUE:
        return LED_BLUE;
    default:
        break;
    }
    return LED_NC;
}

std::string CameraLED::GetColorStr(ColorLED color)
{
    switch (color)
    {
    case ColorLED::RED:
        return "RED";
    case ColorLED::GREEN:
        return "GREEN";
    case ColorLED::BLUE:
        return "BLUE";
    default:
        break;
    }
    return LED_NC;
}

void CameraLED::PrintStatus()
{
    system("clear");
    std::string whiteColor = GetColorCodeStr(ColorLED::UNKNOWN);
    std::cout << whiteColor << "State: ";
    std::cout << GetColorCodeStr(m_Color) << GetStateStr(m_State) << std::endl;
    std::cout << whiteColor << "Rate: ";
    std::cout << GetColorCodeStr(m_Color) << std::to_string(m_Rate) << std::endl;
    std::cout << whiteColor;
}
