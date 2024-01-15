#include "Client_manager.h"

Client_manager::Client_manager(Connection_manager connection_manager): cm(connection_manager) {
}
Client_manager::Client_manager() = default;

shared_ptr<Client_data> Client_manager::find_client_by_socket(int client_socket) {

    shared_ptr<Client_data> shared_ptr = nullptr;

    for (const auto& client_ptr : clients) {
        if (client_ptr->get_socket() == client_socket) {
            shared_ptr = client_ptr;
            break;
        }
    }

    return shared_ptr;
}

shared_ptr<Client_data> Client_manager::find_client_by_name(const string& name) {

    shared_ptr<Client_data> shared_ptr = nullptr;

    for (const auto& client_ptr : clients) {
        if (client_ptr->get_client_name() == name) {
            shared_ptr = client_ptr;
            break;
        }
    }

    return shared_ptr;
}

int Client_manager::login_client(const string& login, int client_socket) {
    shared_ptr<Client_data> client = find_client_by_name(login);
    if(client) {
        return try_reconnect(client, client_socket);
    }
    else {
        if(login.size() <= NAME_MAX_LENGTH && login.size() >= NAME_MIN_LENGTH && login != NO_VALID_NAME) {
            client = make_shared<Client_data>(client_socket, login);
            client->set_status(LOGIN_STATUS);
            clients.push_back(client);

            cm.send_message("OK 0 " + to_string(PING_INTERVAL) + " " + to_string(MAX_MISSED_PINGS), client_socket);     //successfully added
            return 0;
        }
        else {
            cm.send_message("ERR -2", client_socket);    // wrong login format
            return -2;
        }
    }
}

int Client_manager::try_reconnect(const shared_ptr<Client_data>& client, int client_socket) {
    if(client->get_status() != RECONNECTING_STATUS) {
        cm.send_message("ERR -1", client_socket);
        return -1;
    }

    cout << "reconnecting client " << client->get_client_name() << endl;
    client->set_alive_state(true);
    client->set_socket(client_socket);
    client->reset_pings();

    if(client->game == nullptr) {
        client->set_status(LOGIN_STATUS);
        cm.send_message("OK " + std::to_string(LOGIN_STATUS) + " "
                        + to_string(PING_INTERVAL) + " " + to_string(MAX_MISSED_PINGS) , client_socket);
        return LOGIN_STATUS;
    }
    else {
        client->set_status(IN_LOBBY_STATUS);
        cm.send_message("OK " + std::to_string(IN_LOBBY_STATUS) + " "
                        + to_string(PING_INTERVAL) + " " + to_string(MAX_MISSED_PINGS) , client_socket);

        return IN_LOBBY_STATUS;
    }
}

void Client_manager::client_leaves(const shared_ptr<Client_data>& client) {
    if(client) {
        client->set_status(RECONNECTING_STATUS);
        client->leave_time = std::chrono::high_resolution_clock::now();
        cout << "client " << client->get_client_name() << " LEFT server " << TIME_TO_RECONNECT << "s to reconnect" << endl;
    }
}

bool Client_manager::ping_cycle(chrono::high_resolution_clock::time_point& last_ping_time) {

    chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - last_ping_time);
    long seconds = duration.count();

    if(seconds >= PING_INTERVAL) {
        ping_all();
        reset_pings();
        last_ping_time = std::chrono::high_resolution_clock::now();
        return true;
    }

    return false;
}

void Client_manager::ping_all() {
    for(const shared_ptr<Client_data>& client : clients) {
        if(client->get_status() != INVALID_STATUS && client->get_status() != RECONNECTING_STATUS)
            cm.send_message("PING", client->get_socket());
    }
}

void Client_manager::reset_pings() {
    for(shared_ptr<Client_data>& client : clients) {
        if(client->get_status() != INVALID_STATUS && client->get_status() != RECONNECTING_STATUS) {
            if(client->is_alive())
                client->set_alive_state(false);
            else {
                client->add_missed_ping();
                cout << client->get_client_name() << " missed ping" << endl;
            }
        }
    }
}

std::vector<std::shared_ptr<Client_data>> Client_manager::get_inactive() {
    vector<shared_ptr<Client_data>> inactive;
    for(const shared_ptr<Client_data>& client : clients)
        if(client->get_status() != RECONNECTING_STATUS && client->get_pings_missed() > MAX_MISSED_PINGS)
            inactive.push_back(client);

    return inactive;
}

void Client_manager::kick_inactive(const vector<shared_ptr<Client_data>>& inactive) {
    for(const shared_ptr<Client_data>& client : inactive)
            client_leaves(client);
}

vector<shared_ptr<Client_data>> Client_manager::select_dead_clients() {
    vector<shared_ptr<Client_data>> dead;
    chrono::high_resolution_clock::time_point time_now = std::chrono::high_resolution_clock::now();

    for(const shared_ptr<Client_data>& client : clients) {
        if(client->get_status() == RECONNECTING_STATUS) {

            auto duration = std::chrono::duration_cast<std::chrono::seconds>(time_now - client->leave_time);
            long seconds = duration.count();

            if(seconds >= TIME_TO_RECONNECT)
                dead.push_back(client);
        }
    }
    return dead;
}

std::vector<shared_ptr<Client_data>> Client_manager::lobby_change_affected() {
    vector<shared_ptr<Client_data>> affected;
    for(const shared_ptr<Client_data>& client : clients)
        if(client->get_status() == LOGIN_STATUS)
            affected.push_back(client);

    return affected;
}

void Client_manager::user_disconnect(shared_ptr<Client_data>& client) {
    clients.erase(remove(clients.begin(), clients.end(), client), clients.end());

    cout << "DELETING USER " << client->get_client_name() << " (" << client.use_count() << ") refs left" << std::endl;
    client.reset();
}

void Client_manager::disconnect_dead(const vector<shared_ptr<Client_data>>& dead) {
    for(shared_ptr<Client_data> client : dead)
        user_disconnect(client);
}





