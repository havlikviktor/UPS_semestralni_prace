#ifndef LOBBY_MANAGER_H
#define LOBBY_MANAGER_H

#include <memory>
#include <vector>
#include "../Connection_manager/Connection_manager.h"
#include "../../Client_data/Client_data.h"

class Connection_manager;

using namespace std;

class Lobby_manager {

public:
    Lobby_manager();
    explicit Lobby_manager(Connection_manager connection_manager);

    int send_lobby(const std::shared_ptr<Client_data>& client);

    int join_lobby(const shared_ptr<Client_data>& client,const string& lobby_name);

    int leave_lobby(const shared_ptr<Client_data>& client);

    int get_lobby_info(const shared_ptr<Client_data>& client);

    void add_lobby(const string &lobby_name);

    void lobby_change_notify(const std::vector<shared_ptr<Client_data>>& clients);

    void kick_inactive(const vector<shared_ptr<Client_data>> &inactive);

    void client_leaves(const shared_ptr<Client_data> &client);

    void disconnect_dead(const vector<shared_ptr<Client_data>> &dead);

private:
    Connection_manager cm;
    vector<shared_ptr<Lobby>> lobbies;

    static vector<string> create_lobby_format(const shared_ptr<Client_data> &client);

    int send_lobby(const shared_ptr<Client_data> &client, const string& prefix);

    void notify_others(const shared_ptr<Lobby> &lobby, const string &client_name);
};
#endif //LOBBY_MANAGER_H
