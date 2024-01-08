#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <list>
#include "../Connection_manager/Connection_manager.h"
#include "../../Client_data/Client_data.h"

class Connection_manager;
class Client_data;

using namespace std;

class Client_manager {
public:
    Client_manager();
    explicit Client_manager(Connection_manager connection_manager);
    int login_client(const string& login,int  client_socket);
    shared_ptr<Client_data> find_client_by_socket(int socket);
    shared_ptr<Client_data> find_client_by_name(const string &name);

    void client_leaves(const shared_ptr<Client_data>& client);

    bool ping_cycle(chrono::high_resolution_clock::time_point& last_ping_time);

    vector<shared_ptr<Client_data>> select_dead_clients();

    std::vector<shared_ptr<Client_data>> lobby_change_affected();

    std::vector<std::shared_ptr<Client_data>> get_inactive();

    void kick_inactive(const vector<shared_ptr<Client_data>> &inactive);

    void disconnect_dead(const vector<shared_ptr<Client_data>> &inactive);

private:
    Connection_manager cm;
    list<shared_ptr<Client_data>> clients;

    int try_reconnect(const shared_ptr<Client_data>& client, int client_socket);

    void ping_all();

    void reset_pings();

    void user_disconnect(shared_ptr<Client_data>& sharedPtr);
};

#endif// CLIENT_MANAGER_H
