#include "pch.h"

#include "ConnectionManager.h"
#include "ClientConnection.h"

extern bool RunServer;

std::mutex ConnectionManager::g_Mutex;

std::shared_ptr<CommandResult> SetLedState(std::string args);
std::shared_ptr<CommandResult> GetLedState(std::string args);
std::shared_ptr<CommandResult> SetLedColor(std::string args);
std::shared_ptr<CommandResult> GetLedColor(std::string args);
std::shared_ptr<CommandResult> SetLedRate(std::string args);
std::shared_ptr<CommandResult> GetLedRate(std::string args);

std::map<std::string, StateLED> ConnectionManager::KEYS_STATE_LED({
    {"ON", StateLED::ON},
    {"OFF", StateLED::OFF}
});

std::map<std::string, ColorLED> ConnectionManager::KEYS_COLOR_LED({
    {"RED", ColorLED::RED},
    {"GREEN", ColorLED::GREEN},
    {"BLUE", ColorLED::BLUE}
});

std::map<std::string, ConnectionManager::Executer> ConnectionManager::COMMAND_EXECUTERS({
    {"SET-LED-STATE", SetLedState},
    {"GET-LED-STATE", GetLedState},
    {"SET-LED-COLOR", SetLedColor},
    {"GET-LED-COLOR", GetLedColor},
    {"SET-LED-RATE", SetLedRate},
    {"GET-LED-RATE", GetLedRate},
});

ConnectionManager::ConnectionManager()
{
}

ConnectionManager::~ConnectionManager()
{
}

std::string ConnectionManager::GetToken(std::string& str, unsigned int& pos)
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

void *ConnectionManager::Run(void* managerPtr)
{
    ConnectionManager* manager = (ConnectionManager*)managerPtr;
    int timeout = 100;
    CameraLED::Get()->PrintStatus();
    while (RunServer)
    {
        int index = 0;

        int nfds = manager->m_Connections.size();
        struct pollfd fds[nfds];
        memset(fds, 0 , sizeof(fds));
        {
            std::lock_guard<std::mutex> lock(g_Mutex);
            for (auto& val : manager->m_Connections)
            {
                fds[index].fd = val.first;
                fds[index].events = POLLIN;
                index++;
            }
        }

        auto rc = poll(fds, nfds, timeout);
        if (rc < 0)
        {
          printf("Error poll: %s\n", strerror(errno));
          break;
        }

        if (rc == 0)
        {
          continue;
        }

        std::lock_guard<std::mutex> lock(g_Mutex);
        for (int i = 0; i < nfds; i++)
        {
            if (fds[i].revents == 0)
                continue;

            if (fds[i].revents != POLLIN)
            {
                printf("Error revents: %s\n", strerror(fds[i].revents));
                auto FindIter = manager->m_Connections.find(fds[i].fd);
                if (FindIter != manager->m_Connections.end())
                    manager->m_Connections.erase(FindIter);
                break;
            }

            auto FindIter = manager->m_Connections.find(fds[i].fd);
            if (FindIter != manager->m_Connections.end())
            {
                FindIter->second->ReceiveData();
            }
        }

        for (auto& val : manager->m_Connections)
        {
            auto commandsDeq = val.second->GetCommands();
            if (!commandsDeq.empty())
            {
                for (auto& str : commandsDeq)
                {
                    unsigned int pos = 0;
                    std::string command = ConnectionManager::GetToken(str, pos);
                    std::size_t fpos = command.find("SET");
                    std::string args = "";
                    if (fpos != std::string::npos)
                        args = GetToken(str, ++pos);
                    std::string resultCommand = manager->ExecuteCommand(command, args);
                    val.second->AddAnswer(resultCommand);
                    val.second->SendData();
                    CameraLED::Get()->PrintStatus();
                }
                val.second->ClearCommands();
            }
        }
        auto iter = manager->m_Connections.begin();
        while (iter != manager->m_Connections.end())
        {
            if (!iter->second->IsConnected())
                iter = manager->m_Connections.erase(iter);
            else
                ++iter;
        }
    }
}

void ConnectionManager::CreateConnection(int socket)
{
    std::lock_guard<std::mutex> lock(g_Mutex);
    auto FindIter = m_Connections.find(socket);
    if (FindIter != m_Connections.end())
    {
        FindIter->second.reset();
    }

    std::shared_ptr<ClientConnection> connection = std::make_shared<ClientConnection>();
    connection->SetSocket(socket);

    m_Connections[socket] = connection;
}

std::string ConnectionManager::ExecuteCommand(const std::string& command, const std::string& arg)
{
    auto FindCommand = COMMAND_EXECUTERS.find(command);
    if (FindCommand == COMMAND_EXECUTERS.end())
    {
        return "FAILED";
    }
    auto Func = FindCommand->second;
    auto Result = Func(arg);
    std::string answer = "FAILED";
    switch (Result->GetResult())
    {
    case EResult::RES_OK:
        answer = "OK";
        break;
    case EResult::RES_FAILED:
        answer = "FAILED";
        break;
    }
    switch (Result->GetType())
    {
    case ECommand::GET_STATE:
    {
        std::shared_ptr<GetLedStateResult> StateResult = std::static_pointer_cast<GetLedStateResult>(Result);
        if (!StateResult)
        {
            answer = "FAILED";
            break;
        }
        StateLED state = StateResult->GetState();
        std::string strState = CameraLED::GetStateStr(state);
        answer += std::string(" " + strState);
    }
    break;
    case ECommand::GET_COLOR:
    {
        std::shared_ptr<GetLedColorResult> ColorResult = std::static_pointer_cast<GetLedColorResult>(Result);
        if (!ColorResult)
        {
            answer = "FAILED";
            break;
        }
        ColorLED color = ColorResult->GetColor();
        std::string strColor = CameraLED::GetColorStr(color);
        answer += std::string(" " + strColor);
    }
    break;
    case ECommand::GET_RATE:
    {
        std::shared_ptr<GetLedRateResult> RateResult = std::static_pointer_cast<GetLedRateResult>(Result);
        if (!RateResult)
        {
            answer = "FAILED";
            break;
        }
        int rate = RateResult->GetRate();
        std::string strRate = std::to_string(rate);
        answer += std::string(" " + strRate);
    }
    break;
    case ECommand::SET_STATE:
    case ECommand::SET_COLOR:
    case ECommand::SET_RATE:
    break;
    }

    return answer;
}

std::shared_ptr<CommandResult> SetLedState(std::string args)
{
    EResult result = EResult::RES_FAILED;
    if (!args.empty())
    {
        auto FindState = ConnectionManager::KEYS_STATE_LED.find(args);
        if (FindState != ConnectionManager::KEYS_STATE_LED.end())
        {
            CameraLED::Get()->SetState(FindState->second);
            result = EResult::RES_OK;
        }
    }
    return std::make_shared<CommandResult>(ECommand::SET_STATE, result);
}

std::shared_ptr<CommandResult> GetLedState(std::string args)
{
    return std::make_shared<GetLedStateResult>(ECommand::GET_STATE, EResult::RES_OK, CameraLED::Get()->GetState());
}

std::shared_ptr<CommandResult> SetLedColor(std::string args)
{
    EResult result = EResult::RES_FAILED;
    if (!args.empty())
    {
        auto FindColor = ConnectionManager::KEYS_COLOR_LED.find(args);
        if (FindColor != ConnectionManager::KEYS_COLOR_LED.end())
        {
            CameraLED::Get()->SetColor(FindColor->second);
            result = EResult::RES_OK;
        }
    }
    return std::make_shared<CommandResult>(ECommand::SET_COLOR, result);
}

std::shared_ptr<CommandResult> GetLedColor(std::string args)
{
    return std::make_shared<GetLedColorResult>(ECommand::GET_COLOR, EResult::RES_OK, CameraLED::Get()->GetColor());
}

std::shared_ptr<CommandResult> SetLedRate(std::string args)
{
    EResult result = EResult::RES_FAILED;
    if (!args.empty())
    {
        int rate = std::stoi(args);
        if (rate == CameraLED::ClampRate(rate))
        {
            CameraLED::Get()->SetRate(rate);
            result = EResult::RES_OK;
        }
    }
    return std::make_shared<CommandResult>(ECommand::SET_RATE, result);
}

std::shared_ptr<CommandResult> GetLedRate(std::string args)
{
    return std::make_shared<GetLedRateResult>(ECommand::GET_RATE, EResult::RES_OK, CameraLED::Get()->GetRate());
}
