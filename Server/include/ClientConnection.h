#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H


class ClientConnection
{
public:
    ClientConnection();
    virtual ~ClientConnection();

    int GetSocket() const { return m_Socket; }
    void SetSocket(int val);
    void ReceiveData();
    void SendData();
    void AddAnswer(const std::string& data);
    int GetCountCommands() const;
    std::deque<std::string> GetCommands();
    void ClearCommands();
    bool IsConnected() const { return m_Connected; }
private:
    bool m_Connected;
    int m_Socket;
    std::vector<unsigned char> m_Buffer;
    int m_StartPos, m_EndPos;
    std::deque<std::string> m_Answers;
    std::deque<std::string> m_Commands;
};

#endif // CLIENTCONNECTION_H
