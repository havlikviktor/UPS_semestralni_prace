#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include "../../Constants.h"
#include "../../Client_data/Client_data.h"
#include "../Connection_manager/Connection_manager.h"
#include "../Client_manager/Client_manager.h"
#include "../Lobby_manager/Lobby_manager.h"
#include "../Game_manager/Game_manager.h"

#include <vector>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <chrono>
#include <sys/ioctl.h>
#include <unistd.h>


class Connection_manager;
class Client_manager;
class Lobby_manager;

using namespace std;

class Server_manager {
public:
    Server_manager(int port,in_addr_t address);

    void start_server(int max_clients);

    void add_lobby(const string& lobby_name);

private:
    int server_socket;
    Connection_manager con_m;
    Client_manager cm;
    Lobby_manager lm;
    Game_manager gm;

    void create_managers();

    int request_handler(list<string> request, int client_socket);

    void kick_inactive(const std::vector<shared_ptr<Client_data>>& inactive);

    void disconnect_dead(const vector<shared_ptr<Client_data>> &inactive, fd_set *clients_sockets);

    void handle_request(int client_socket, fd_set *clients_sockets);

    void kill_socket(int socket, fd_set *client_socks);
};




#endif //SERVER_MANAGER_H
