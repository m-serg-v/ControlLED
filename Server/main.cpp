#include "pch.h"

#include "CameraLED.h"
#include "ConnectionManager.h"

#include <pthread.h>

#define DefaultPort 7777

bool RunServer;

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "rus");
    unsigned short port = DefaultPort;
    if (argc == 2)
    {
        port = std::stoi(argv[1]);
    }
    
    RunServer = true;
    printf("Init server...\n");
    int server_sock;
    struct sockaddr_in server_addr;
    printf("Create socket: ");
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        printf("Error: %s\n", strerror(errno));
        return -1;
    }
    printf("Ok\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    printf("Bind socket: ");
    if (bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Error: %s\n", strerror(errno));
        return -1;
    }
    printf("Ok\n");

    listen(server_sock, 5);

    printf("Server initialized. Port: %d\n", port);

    std::shared_ptr<ConnectionManager> manager = std::make_shared<ConnectionManager>();

    pthread_t tid;
    auto funcPtr = ConnectionManager::Run;
    int err = pthread_create(&tid, NULL, funcPtr, manager.get());
    if (err != 0)
    {
        printf("Error: %s\n", strerror(err));
        return -1;
    }
    while (RunServer)
    {
        sockaddr_in client_addr;
        int client_sock;
        int size_addr = sizeof(client_addr);

        client_sock = accept(server_sock, (sockaddr*)&client_addr, (socklen_t*)&size_addr);
        if (client_sock < 0)
        {
            printf("Error accept: %s", strerror(errno));
            return -1;
        }
        printf("Client connected: %d\n", client_sock);
        manager->CreateConnection(client_sock);
    }

    return 0;
}
