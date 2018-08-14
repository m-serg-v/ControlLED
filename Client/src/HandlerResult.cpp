#include "pch.h"

#include "HandlerResult.h"

std::map<std::string, StateLED> HandlerResult::KEYS_STATE_LED(
{
    {"ON", StateLED::ON},
    {"OFF", StateLED::OFF}
});

std::map<std::string, ColorLED> HandlerResult::KEYS_COLOR_LED(
{
    {"RED", ColorLED::RED},
    {"GREEN", ColorLED::GREEN},
    {"BLUE", ColorLED::BLUE}
});

std::map<std::string, EResult> HandlerResult::KEYS_RESULT(
{
    {"OK", EResult::RES_OK},
    {"FAILED", EResult::RES_FAILED}
});

HandlerResult::HandlerResult()
{
}

HandlerResult::~HandlerResult()
{
}

void HandlerResult::OnResultCommand(ECommand type, std::string result)
{
    unsigned int pos = 0;
    std::string token = GetToken(result, pos);
    auto FindIter = KEYS_RESULT.find(token);
    if (FindIter == KEYS_RESULT.end())
    {
        std::cout << "Error result format\n";
        return;
    }

    if (FindIter->second == EResult::RES_FAILED)
    {
        std::cout << FindIter->first << std::endl;
        return;
    }

    std::string resultStr = FindIter->first;

    token = GetToken(result, pos);
    switch (type)
    {
    case ECommand::GET_STATE:
    {
        auto Iter = KEYS_STATE_LED.find(token);
        if (Iter == KEYS_STATE_LED.end())
        {
            resultStr == "Error result format\n";
            break;
        }
        resultStr += std::string(" " + token);
    }
    break;
    case ECommand::GET_COLOR:
    {
        auto Iter = KEYS_COLOR_LED.find(token);
        if (Iter == KEYS_COLOR_LED.end())
        {
            resultStr == "Error result format\n";
            break;
        }
        resultStr += std::string(" " + token);
    }
    break;
    case ECommand::GET_RATE:
    {
        resultStr += std::string(" " + token);
    }
    break;
    }

    std::cout << resultStr << std::endl;
}
