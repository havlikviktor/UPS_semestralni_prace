#include "Managers/Server_manager/Server_manager.h"

int main(int argc, char *argv[]) {
    if (argc < 5 || (argc - 3) % 2 != 0) {
        std::cerr << "wrong args format: " << argv[0] << " -a <address> -p <port> -l <lobby1> [<lobby2> ...]" << std::endl;
        return 1;
    }

    std::string address;
    int port;
    std::vector<std::string> lobbies;

    for (int i = 1; i < argc; i += 2) {
        std::string option = argv[i];

        if (option == "-a") {
            address = argv[i + 1];
        } else if (option == "-p") {
            port = std::atoi(argv[i + 1]);
        } else if (option == "-l") {
            for (int j = i + 1; j < argc && argv[j][0] != '-'; ++j) {
                lobbies.emplace_back(argv[j]);
            }
            break;
        } else {
            std::cerr << "Invalid option: " << option << std::endl;
            return 1;
        }
    }

    // Your Server_manager initialization code here

    std::cout << "Address: " << address << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Lobbies:";
    for (const std::string& lobby : lobbies) {
        std::cout << " " << lobby;
    }
    std::cout << std::endl;

    // Your Server_manager initialization code here

    return 0;
}