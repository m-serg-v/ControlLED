#include "pch.h"
#include "Connection.h"

#define BUFFER_SIZE 1024

std::map<std::string, ECommand> Connection::KEYS_COMMANDS(
{
    {"SET-LED-STATE", ECommand::SET_STATE},
    {"GET-LED-STATE", ECommand::GET_STATE},
    {"SET-LED-COLOR", ECommand::SET_COLOR},
    {"GET-LED-COLOR", ECommand::GET_COLOR},
    {"SET-LED-RATE", ECommand::SET_RATE},
    {"GET-LED-RATE", ECommand::GET_RATE},
});

Connection::Connection()
{
    m_Connected = false;
}

Connection::~Connection()
{
}


void Connection::Init(std::string addr, unsigned short port)
{
    struct sockaddr_in server_addr;
    m_Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_Socket < 0)
    {
        printf("Error: %s\n", strerror(errno));
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(addr.c_str());
    server_addr.sin_port = htons(port);

    if (connect(m_Socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Error: %s\n", strerror(errno));
        return;
    }
    m_Connected = true;
    std::cout << "Connected\n";
}

void Connection::AddListener(const std::shared_ptr<IListener>& listener)
{
    if (!listener)
        return;

    m_Listeners.push_back(listener);
}

void Connection::TryExecuteCommand(std::string command)
{
    unsigned int pos = 0;
    std::string token = GetToken(command, pos);

    auto FindIter = KEYS_COMMANDS.find(token);
    if (FindIter == KEYS_COMMANDS.end())
    {
        std::cout << "Command " << token << " not found\n";
        return;
    }

    switch (FindIter->second)
    {
    case ECommand::SET_STATE:
    case ECommand::SET_COLOR:
    case ECommand::SET_RATE:
    {
        token = GetToken(command, pos);
        if (token.empty())
        {
            std::cout << "Argument not found\n";
            return;
        }
    }
    break;
    }

    if (!SendCommand(command))
        return;

    RecvResult(FindIter->second);
}

bool Connection::SendCommand(std::string command)
{
    command.push_back('\n');
    int size_sent = 0;
    while (size_sent < (int)command.size())
    {
        int snd = send(m_Socket, &command[size_sent], command.size() - size_sent, 0);
        if (snd < 0)
        {
            std::cout << "Send error: " << strerror(snd) << std::endl;
            m_Connected = false;
            return false;
        }
        if (snd == 0)
        {
            std::cout << "Server closed the connection\n";
            m_Connected = false;
            return false;
        }
        size_sent += snd;
    }
    return true;
}

bool Connection::RecvResult(ECommand type)
{
    struct pollfd fds;
    int nfds = 1;
    int timeout = 10 * 1000; // 10 sec for answer
    memset(&fds, 0 , sizeof(fds));
    fds.fd = m_Socket;
    fds.events = POLLIN;
    std::string result = "";
    while (true)
    {
        int rc = poll(&fds, nfds, timeout);
        if (rc < 0)
        {
            std::cout << "Error poll: " << strerror(errno) << std::endl;
            m_Connected = false;
            break;
        }

        if (rc == 0)
        {
            std::cout << "Timeout wait answer. Close connection\n";
            m_Connected = false;
            break;
        }

        if (fds.revents != POLLIN)
        {
            std::cout << "Error revents: " << std::to_string(fds.revents) << std::endl;
            m_Connected = false;
            break;
        }

        char buffer[BUFFER_SIZE] = "";
        int rcv = recv(m_Socket , buffer , BUFFER_SIZE , 0);
        if( rcv < 0)
        {
            std::cout << "Recv error: " << strerror(errno) << std::endl;
            m_Connected = false;
            break;
        }
        else if (rcv == 0)
        {
            std::cout << "Close connection\n";
            m_Connected = false;
            break;
        }

        result += std::string(buffer, buffer + rcv);

        auto EndPos = result.find('\n');
        if (EndPos != std::string::npos)
        {
            EmitListeners(type, std::string(result.begin(), result.begin() + EndPos));
            return true;
        }
    }

    return false;
}

void Connection::EmitListeners(ECommand type, std::string command)
{
    auto iter = m_Listeners.begin();
    while ( iter != m_Listeners.end())
    {
        std::shared_ptr<IListener> listenerPtr = iter->lock();
        if (listenerPtr)
        {
            listenerPtr->OnResultCommand(type, command);
            ++iter;
        }
        else
            iter = m_Listeners.erase(iter);
    }
}
