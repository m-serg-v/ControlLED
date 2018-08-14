#include "pch.h"

#include "Connection.h"
#include "HandlerResult.h"

#define DefaultPort 7777
#define DefaultAddress "127.0.0.1"

void print_info_run()
{
    std::cout << "Client [address] [port]" << std::endl;
}

void printf_info_commands()
{
    std::cout << "Available commands:\n" << std::endl;
    std::cout << "Command\t\tArgument\t\tResult\t\t\t\tInfo" << std::endl;
    std::cout << "set-led-state\ton,off\t\t\tOK, FAILED\t\t\tTurn on/off LED" << std::endl;
    std::cout << "get-led-state\t------\t\t\tOK on|off, FAILED\t\tGet state LED" << std::endl;
    std::cout << "set-led-color\tred, green, blue\tOK, FAILED\t\t\tChange color LED" << std::endl;
    std::cout << "get-led-color\t------\t\t\tOK red|green|blue, FAILED\tGet color LED" << std::endl;
    std::cout << "set-led-rate\t0...5\t\t\tOK, FAILED\t\t\tChange rate LED" << std::endl;
    std::cout << "get-led-rate\t--------\t\tOK 0...5, FAILED\t\tGet rate LED" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Wrong count arguments\n");
        print_info_run();
        return -1;
    }

    std::string server_addr = argv[1];
    unsigned short port = std::stoi(argv[2]);

    std::shared_ptr<Connection> Client = std::make_shared<Connection>();
    Client->Init(server_addr, port);

    std::shared_ptr<HandlerResult> Handler = std::make_shared<HandlerResult>();
    Client->AddListener(Handler);

    printf_info_commands();

    while (true)
    {
        std::string input_data = "";
        if (!Client->IsConnected())
        {
            std::cout << "Not connected. Try connect again? (yes/no): ";
            std::string result = "";
            std::cin >> result;
            std::locale loc;

            std::transform(result.begin(), result.end(), result.begin(), [&loc](const char& c)
            {
                return std::tolower(c, loc);
            });
            if (result.compare("yes") == 0)
            {
                Client->Init(server_addr, port);
                continue;
            }
            break;
        }

        std::getline(std::cin, input_data);

        if (input_data.compare("quit")==0)
            break;
        Client->TryExecuteCommand(input_data);
    }

    return 0;
}
