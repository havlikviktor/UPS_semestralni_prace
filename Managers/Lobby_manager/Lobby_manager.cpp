#include "Lobby_manager.h"

Lobby_manager::Lobby_manager() = default;
Lobby_manager::Lobby_manager(Connection_manager connection_manager): cm(connection_manager) {}


int Lobby_manager::send_lobby(const shared_ptr<Client_data> &client) {
    return send_lobby(client,"OK");
}

int Lobby_manager::send_lobby(const std::shared_ptr<Client_data>& client, const string& prefix) {
    vector<string> send_back;

    for(const shared_ptr<Lobby>& lobby : lobbies) {
        string lobby_info = lobby->get_name() + ";"  + lobby->get_leader() + ";" + to_string(lobby->get_inside_count());
        send_back.push_back(lobby_info);
    }

    send_back.push_back(to_string(LOGIN_STATUS));
    send_back.emplace_back(prefix);

    cm.send_message(send_back, client->get_socket());
    return 1;
}

int Lobby_manager::join_lobby(const shared_ptr<Client_data>& client,const string& lobby_name) {
    int result = -4;

    for (const shared_ptr<Lobby>& lobby : lobbies) {
        if (lobby->get_name() == lobby_name) {
            result = lobby->add_player(client);
            client->set_lobby(lobby);
            client->set_status(IN_LOBBY_STATUS);
        }
    }

    if(result == 1) {
        cm.send_message("OK 1", client->get_socket());
        notify_others(client->get_lobby(), client->get_client_name());
        return result;
    }

    else {
        cm.send_message("ERR " + to_string(result), client->get_socket());
        return result;
    }
}

void Lobby_manager::notify_others(const shared_ptr<Lobby>& lobby, const string& client_name) {
    if(lobby == nullptr)
        return;

    vector<shared_ptr<Client_data>> in_lobby = lobby->in;

    for(const shared_ptr<Client_data>& c : in_lobby) {
        if(c->get_client_name() != client_name) {
            vector<string> notification = create_lobby_format(c);
            notification.emplace_back(to_string(IN_LOBBY_STATUS));
            notification.emplace_back("UPDATE");
            cm.send_message(notification , c->get_socket());
        }
    }
}

int Lobby_manager::leave_lobby(const shared_ptr<Client_data>& client) {
    shared_ptr<Lobby> lobby = client->get_lobby();
    int result = client->leave_lobby();

    if(result == 1) {
        cm.send_message("OK " + to_string(result), client->get_socket());
        notify_others(lobby, client->get_client_name());
    }

    else
        cm.send_message("ERR " + to_string(result), client->get_socket());

    return result;
}

int Lobby_manager::get_lobby_info(const shared_ptr<Client_data>& client) {
    vector<string> send_back;
    shared_ptr<Lobby> lobby = client->get_lobby();

    if(lobby == nullptr) {
        cm.send_message("ERR -1",client->get_socket());
        return -1;              // hrac neni v zadnme lobby
    }

    send_back = create_lobby_format(client);

    send_back.push_back(to_string(lobby->get_inside_count()));
    send_back.emplace_back("OK");

    cm.send_message(send_back, client->get_socket());
    return 1;
}

vector<string> Lobby_manager::create_lobby_format(const shared_ptr<Client_data>& client) {
    vector<string> format = client->get_lobby()->get_player_names();

    if(client->get_client_name() == client->get_lobby()->get_leader())
        format.emplace_back("leader");
    else
        format.emplace_back("member");

    return format;
}


void Lobby_manager::add_lobby(const string &lobby_name) {
    for(const shared_ptr<Lobby>& lobby : lobbies) {
        if(lobby->get_name() == lobby_name) {
            cout << "lobby with name " << lobby_name << " already exists" << endl;
            return;
        }
    }

    shared_ptr<Lobby> new_lobby = make_shared<Lobby>(lobby_name);

    lobbies.push_back(new_lobby);
}

void Lobby_manager::lobby_change_notify(const std::vector<shared_ptr<Client_data>>& clients) {
    for(const shared_ptr<Client_data>& client : clients) {
        send_lobby(client, "UPDATE");
    }
}

void Lobby_manager::kick_inactive(const vector<shared_ptr<Client_data>> &inactive) {
    for(const shared_ptr<Client_data>& client : inactive) {
        client_leaves(client);
    }
}

void Lobby_manager::disconnect_dead(const vector<shared_ptr<Client_data>> &dead) {
    kick_inactive(dead);
}

void Lobby_manager::client_leaves(const shared_ptr<Client_data> &client) {
    if(client == nullptr || client->get_lobby() == nullptr || client->game != nullptr)
        return;

    shared_ptr<Lobby> lobby = client->get_lobby();
    client->leave_lobby();
    notify_others(lobby,client->get_client_name());
}


