#ifndef CONNECTION_H
#define CONNECTION_H

class Connection
{
public:
    Connection();
    virtual ~Connection();

    void Init(std::string addr, unsigned short port);
    bool IsConnected() const
    {
        return m_Connected;
    }
    void AddListener(const std::shared_ptr<IListener>& listener);
    void TryExecuteCommand(std::string command);

private:
    bool SendCommand(std::string command);
    bool RecvResult(ECommand type);
    void EmitListeners(ECommand type, std::string command);

private:
    int m_Socket;
    bool m_Connected;
    std::list<std::weak_ptr<IListener>> m_Listeners;

    static std::map<std::string, ECommand> KEYS_COMMANDS;
};

#endif // CONNECTION_H
