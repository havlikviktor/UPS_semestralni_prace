#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include "../../Constants.h"

#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <list>

using namespace std;

class Connection_manager {
public:
    Connection_manager();

    /**
     * reads data from given client socket and slits them into vector by character MSG_SEPARATOR
     * @param client_socket socket from witch data are read
     * @return vector representing incoming message
     *
     * @see recv_msg
     */
    list<string> get_msg_list(int client_socket);

    /**
     * reads data from socket, size of buffer is set to 1024
     * @param client_socket socket from witch data are read
     * @return string read from this socket
     */
    string recv_msg(int client_socket);

    void send_message(std::vector<string> responce, int client_socket);

    /**
     * takes string and sends it to given socket. before prepaves all spaces with MSG_SEPARATOR by using
     * split_by_character method
     * @param responce string that should be send to given socket
     * @param client_socket socket to which message should be sent
     *
     * @see split_by_character()
     */
    void send_message(const string& responce, int client_socket);

    /**
     * @param input string that will be split into list
     * @param character that will be used as separator
     * @return list of string split by @param character
     */
    list<string> split_by_character(const string &input, char character);
};

#endif //CONNECTION_MANAGER_H
