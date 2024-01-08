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

    list<string> get_msg_list(int client_socket);
    string recv_msg(int client_socket);

    void send_message(std::vector<string> responce, int client_socket);
    void send_message(const string& responce, int client_socket);

    list<string> split_by_character(const string &input, char character);
};

#endif //CONNECTION_MANAGER_H
