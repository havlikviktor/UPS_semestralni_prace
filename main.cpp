#include "Managers/Server_manager/Server_manager.h"
#include <iostream>
#include <arpa/inet.h>

    bool load_setup(int argc, char *argv[], in_addr_t* address, int* port, vector<string>* lobbies) {
    for (int i = 1; i < argc; i += 2) {
        std::string option = argv[i];

        if (option == "-a") {
            string a = argv[i + 1];
            *address = inet_addr(argv[i + 1]);

            if (*address == INADDR_NONE) {
                std::cerr << "Invalid ip address: " << a << std::endl;
                return false;
            }
        }
        else if (option == "-p") {
            string a = argv[i + 1];
            *port = std::atoi(argv[i + 1]);
            if (*port == 0 && a[0] != '0') {
                std::cerr << "Invalid port number: " << a << std::endl;
                return false;
            }
        }
        else if (option == "-l") {
            for (int j = i + 1; j < argc && argv[j][0] != '-'; ++j) {
                lobbies->emplace_back(argv[j]);
            }
            break;
        }
        else {
            std::cerr << "Invalid argument option: " << option << std::endl;
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::cerr << "wrong args format: " << argv[0] << " -a <address> -p <port> -l <lobby1> [<lobby2> ...]" << std::endl;
        return 1;
    }
    in_addr_t address;
    int port;
    std::vector<std::string> lobbies;

    if(!load_setup(argc, argv, &address, &port, &lobbies))
        return 1;

    struct in_addr addr;
    addr.s_addr = address;
    const char* result = inet_ntoa(addr);

    printf("IPv4 address: %s\n", result);
    std::cout << "Port: " << port << std::endl;
    std::cout << "Lobbies:";
    for (const std::string& lobby : lobbies) {
        std::cout << " " << lobby;
    }
    std::cout << std::endl;

    Server_manager sm(port, address);

    for(const string& lobby_name : lobbies) {
        if(lobby_name.size() >= LOBBY_NAME_MIN_LENGTH && lobby_name.size() <= LOBBY_NAME_MAX_LENGTH)
        sm.add_lobby(lobby_name);
    }

    sm.start_server(MAX_CLIENTS);


    return 0;
}