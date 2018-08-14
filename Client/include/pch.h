#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/poll.h>

#include <arpa/inet.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <memory>
#include <algorithm>

enum class ECommand
{
    SET_STATE,
    GET_STATE,
    SET_COLOR,
    GET_COLOR,
    SET_RATE,
    GET_RATE
};

class IListener
{
public:
    virtual void OnResultCommand(ECommand type, std::string result) = 0;
};

inline std::string GetToken(std::string& str, unsigned int& pos)
{
    std::locale loc;
    std::string token = "";
    for (; pos < str.size(); ++pos)
    {
        if (str[pos] == '\t' ||
                str[pos] == '\n' ||
                str[pos] == ' ')
        {
            if (token.empty())
                continue;
            else
                return token;
        }
        token.push_back(std::toupper(str[pos], loc));
    }
    return token;
}

#endif // PCH_H_INCLUDED
