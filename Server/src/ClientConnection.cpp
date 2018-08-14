#include "pch.h"

#include "ClientConnection.h"

#define BUFFER_SIZE 1024

ClientConnection::ClientConnection()
    : m_Buffer(BUFFER_SIZE, 0)
{
    m_StartPos = 0, m_EndPos = 0;
    m_Connected = false;
}

ClientConnection::~ClientConnection()
{
    close(m_Socket);
}

void ClientConnection::SetSocket(int val)
{
    m_Socket = val;
    m_Connected = true;
}

int ClientConnection::GetCountCommands() const
{
    return m_Commands.size();
}

std::deque<std::string> ClientConnection::GetCommands()
{
    return m_Commands;
}

void ClientConnection::ClearCommands()
{
    m_Commands.clear();
}

void ClientConnection::ReceiveData()
{
    int count_used = m_EndPos - m_StartPos;
    int available = BUFFER_SIZE - count_used;
    if (available == 0)
    {
        printf("Overflow memmory buffer\n");
        m_Connected = false;
        return;
    }
    if (m_EndPos == BUFFER_SIZE)
    {
        std::vector<unsigned char> temp(count_used, 0);
        memcpy(&temp[0], (&m_Buffer[m_StartPos] + count_used), count_used);
        memcpy(&m_Buffer[0], &temp[m_StartPos], count_used);
        m_StartPos = 0;
        m_EndPos = count_used;
    }

    int size_data = recv(m_Socket, &m_Buffer[0], available, MSG_DONTWAIT);
    if (size_data == 0)
    {
        printf("Client disconnected\n");
        m_Connected = false;
        return;
    }
    else if (size_data < 0)
    {
        printf("Error recv data: socket: %d error: %s\n", m_Socket, strerror(errno));
        m_Connected = false;
        return;
    }
    m_EndPos += size_data;
    auto iter = std::find_if(m_Buffer.begin() + m_StartPos, m_Buffer.begin() + m_EndPos, [](const unsigned char& val)
    {
        if (val == '\n')
            return true;

        return false;
    });
    if (iter != m_Buffer.begin() + m_EndPos)
    {
        std::string command(m_Buffer.begin() + m_StartPos, iter);
        int PosEnd = (iter - m_Buffer.begin() + 1);
        if (m_EndPos == PosEnd)
            m_EndPos = m_StartPos;
        else
            m_StartPos = PosEnd;
        m_Commands.push_back(command);
    }
}

void ClientConnection::SendData()
{
    while (!m_Answers.empty())
    {
        auto data = m_Answers.front();
        data.push_back('\n');
        auto size_send = send(m_Socket, &data[0], data.size(), 0);
        if (size_send == 0)
        {
            printf("Client disconnected\n");
            m_Connected = false;
            return;
        }
        else if (size_send < 0)
        {
            printf("Error send data: %s\n", strerror(errno));
            m_Connected = false;
            return;
        }
        m_Answers.pop_front();
    }
}

void ClientConnection::AddAnswer(const std::string& data)
{
    if (data.empty())
        return;

    m_Answers.push_back(data);
}
