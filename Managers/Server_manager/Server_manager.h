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

    /**
     * server starts listening and accepting incoming messages that are handled by handle_request()
     * @param max_clients maximum number of client that can be connected to server at the same time
     *
     * @see handle_request()
     */
    void start_server(int max_clients);

    /**
     * adds lobby into lost of lobbies
     * @param lobby_name name of new lobby
     *
     * @see Lobby_manager::add_lobby()
     */
    void add_lobby(const string& lobby_name);

private:
    int server_socket;
    Connection_manager con_m;
    Client_manager cm;
    Lobby_manager lm;
    Game_manager gm;

    void create_managers();

    /**
     * handles message
     * @param request
     * @param client_socket the socket from which the request is sent
     * @return positive number if request is done successfully
     * @returns negative number if request cant be completed
     * @returns NO_VALID_REQUEST if request in invalid
     * @returns NOT_REGISTERED_CLIENT if there is request from socket that has no assigned Client_data instance and
     * request was not login request
     */
    int request_handler(list<string> request, int client_socket);

    void kick_inactive(const std::vector<shared_ptr<Client_data>>& inactive);

    void disconnect_dead(const vector<shared_ptr<Client_data>> &inactive, fd_set *clients_sockets);
    /**
     * reads message from client_socket by using Connection_manager. then calls request_handler() for this message.
     * if result of request_handler() is NO_VALID_REQUEST or NOT_REGISTERED_CLIENT it kills this socket by kill_socket()
     * @param client_socket socket to read from
     * @param clients_sockets  set of file descriptors which server is using for accepting connection.
     *
     * @see Connection_manager::get_msg_list()
     * @see request_handler()
     * @see kill_socket()
     */
    void handle_request(int client_socket, fd_set *clients_sockets);

    /**
     * closes connection with this socket, if any Client_data instance is connected to this socket it permanently
     * disconnects this client. done by using disconnect_dead() where client is only object in this vector
     * @param socket that is about to be closed
     * @param client_socks set of file descriptors which server is using for accepting connection.
     *
     * @see disconect_dead()
     */
    void kill_socket(int socket, fd_set *client_socks);
};




#endif //SERVER_MANAGER_H
