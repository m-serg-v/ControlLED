#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "CameraLED.h"

enum class EResult
{
    RES_OK,
    RES_FAILED
};

enum class ECommand
{
    SET_STATE,
    GET_STATE,
    SET_COLOR,
    GET_COLOR,
    SET_RATE,
    GET_RATE
};

class CommandResult
{
    const ECommand m_Type;
    const EResult m_Result;
public:
    CommandResult(ECommand type, EResult res) : m_Type(type), m_Result(res) {}
    EResult GetResult() const { return m_Result; }
    ECommand GetType() const { return m_Type; }
};

class GetLedStateResult : public CommandResult
{
    const StateLED m_State;
public:
    GetLedStateResult(ECommand type, EResult res, StateLED state)
        :
        CommandResult(type, res),
        m_State(state) {}
    StateLED GetState() const { return m_State; }
};

class GetLedColorResult : public CommandResult
{
    const ColorLED m_Color;
public:
    GetLedColorResult(ECommand type, EResult res, ColorLED color)
        :
        CommandResult(type, res),
        m_Color(color) {}
    ColorLED GetColor() const { return m_Color; }
};

class GetLedRateResult : public CommandResult
{
    const int m_Rate;
public:
    GetLedRateResult(ECommand type, EResult res, int rate)
        :
        CommandResult(type, res),
        m_Rate(rate) {}
    int GetRate() const { return m_Rate; }
};

class ConnectionManager
{
public:
    ConnectionManager();
    virtual ~ConnectionManager();

    static void *Run(void* manager);

    void CreateConnection(int socket);

    std::string ExecuteCommand(const std::string& command, const std::string& arg);
private:
    std::map<int, std::shared_ptr<class ClientConnection>> m_Connections;
    static std::mutex g_Mutex;

    static std::string GetToken(std::string& str, unsigned int& pos);

    typedef std::shared_ptr<CommandResult> (*Executer)(std::string);

public:
    static std::map<std::string, Executer> COMMAND_EXECUTERS;
    static std::map<std::string, StateLED> KEYS_STATE_LED;
    static std::map<std::string, ColorLED> KEYS_COLOR_LED;
};

#endif // CONNECTIONMANAGER_H
