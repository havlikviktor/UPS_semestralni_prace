#include "Connection_manager.h"


Connection_manager::Connection_manager() = default;

void Connection_manager::send_message(std::vector<string> responce,int client_socket) {
    if(client_socket == -1)
        return;
    string msg = responce.back();
    responce.pop_back();

    while(!responce.empty()) {
        msg += MSG_SEPARATOR + responce.back();
        responce.pop_back();
    }

    msg.push_back('\n');
    cout << "sending: " << msg ;
    send(client_socket, msg.c_str(), msg.size(), 0);
}

void Connection_manager::send_message(const std::string& responce,int client_socket) {
    list<string> msg_list = split_by_character(responce,' ');

    string msg = msg_list.front();
    msg_list.pop_front();

    for(const string& word: msg_list)
        msg.append(MSG_SEPARATOR + word);

    msg.push_back('\n');
    if(!(msg == "PING\n"))
        cout << "sending: " << msg;
    send(client_socket, msg.c_str(), msg.size(), 0);
}

list<string> Connection_manager::get_msg_list(int client_socket) {
    string msg = Connection_manager::recv_msg(client_socket);
    return split_by_character(msg, MSG_SEPARATOR);
}

std::list<std::string> Connection_manager::split_by_character(const std::string& input, char character) {
    std::list<std::string> result;
    std::istringstream iss(input);
    std::string segment;

    while (std::getline(iss, segment, character)) {
        segment.erase(remove_if(segment.begin(), segment.end(), ::isspace), segment.end()); // odstraneni bilých znaků
        result.push_back(segment);
    }

    return result;
}



string  Connection_manager::recv_msg(int client_socket) {
    const int buffer_size = 1024;
    char buffer[buffer_size];

    ssize_t bytesRead = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    buffer[bytesRead] = '\0';

    std::string data(buffer);

    return data;
}

