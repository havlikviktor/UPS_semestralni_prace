#include "Lobby.h"

#include <utility>

Lobby::Lobby(string name) : lobby_name(std::move(name)), leader("NO LEADER"), in({}){}

std::string Lobby::get_name() const {
    return lobby_name;
}

std::string Lobby::get_leader() const {
    return leader;
}

size_t Lobby::get_inside_count() const {
    return in.size();
}

vector<string> Lobby::get_player_names() const {
    vector<string> names;

    names.push_back(leader);

    for(const shared_ptr<Client_data>& client: in) {
        if(client->get_client_name() == leader)
            continue;
        names.push_back(client->get_client_name());
        if(client->get_client_name().empty())
            client->client_name = "NAME ERR";
    }

    return names;
}


int Lobby::add_player(const shared_ptr<Client_data>& player) {
    if(in.size() >= MAX_IN_LOBBY) {
        return -1;
    }

    for (const shared_ptr<Client_data>& other : in) {
        if(other->get_client_name() == player->get_client_name())
            return -3;
    }

    if(player->get_lobby() != nullptr) {
        return -2;
    }

    if(in.empty())
        leader = player->get_client_name();

    in.push_back(player);
    return 1;
}

void Lobby::change_leader() {
    if(in.empty()){
        leader = "NO LEADER";
    }
    else {
        leader = in.back()->get_client_name();
    }
}

void Lobby::leave(Client_data* player) {

    for (auto it = in.begin(); it != in.end(); ++it) {
        if ((*it)->get_client_name() == player->get_client_name()) {

            string name = (*it)->get_client_name();
            (*it)->set_status(0);
            in.erase(it);

            if(name == leader)
                change_leader();
            break;
        }
    }
}

void Lobby::show() {
    cout << "-------- " << lobby_name << " -----------" << endl;
    for (const shared_ptr<Client_data>& c : in) {
        cout << c->get_client_name() << " : " << c->get_lobby()->get_name() << endl;
    }
    cout << "-----------------------------" << endl;

    if(in.empty())
        cout << "empty lobby " << lobby_name << endl;
}

shared_ptr<Game> Lobby::start_game() {
    shared_ptr<Game> game = make_shared<Game>(lobby_name, in);

    for(const shared_ptr<Client_data>& c : in) {
        c->game = game;
    }

    return game;
}