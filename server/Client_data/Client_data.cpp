#include "Client_data.h"

#include <utility>

using namespace std;


Client_data::Client_data(int socket, string client_name)
        : socket(socket), client_name(std::move(client_name)), game(nullptr), lobby_in(nullptr), status(INVALID_STATUS),
          alive(true), missed_pings(0){
}

Client_data::Client_data()
        : socket(-1), client_name("DEFAULT"), game(nullptr), status(INVALID_STATUS), lobby_in(nullptr),
          alive(false), missed_pings(0) {
}

int Client_data::get_socket() const {
    return socket;
}

string Client_data::get_client_name() const {
    return client_name;
}

shared_ptr<Lobby> Client_data::get_lobby() const {
    return lobby_in;
}

bool Client_data::operator==(const Client_data& other) const {
    return (client_name == other.get_client_name());
}

int Client_data::leave_lobby() {
    if(lobby_in == nullptr) {
        cout << client_name << " no lobby to leave" << endl;
        return -1;
    }
    if(game != nullptr) {
        cout << client_name << " cant leave lobby when still in the game" << endl;
        return -2;
    }


    lobby_in->leave(this);  // informovat lobby o odchodu
    lobby_in.reset();         // opustit lobby
    cout << "client(" << client_name << ") " << "left lobby " << endl;

    return 1;
}

void Client_data::set_lobby(shared_ptr<Lobby> lobby) {
    lobby_in = std::move(lobby);
}

void Client_data::show() {
    cout << client_name << " Lobby: ";
    if(lobby_in == nullptr)
        cout << " no Lobby " << endl;
    else
        cout << lobby_in->get_name() << endl;

    cout<<"game id: " << game->game_id <<endl;
}

void Client_data::set_status(int new_status) {
    status = new_status;
}

int Client_data::get_status() const {
    return status;
}

void Client_data::leave_game() {
    if(game == nullptr) {
        cout << "no game to leave" << endl;
        return;
    }

    game->delete_player(this);
    game.reset();
    cout << "CLIENT " << client_name <<  " LEAVING GAME" << endl ;
}

void Client_data::set_alive_state(bool state) {
    if(state != alive) {
        alive = state;
    }
}

bool Client_data::is_alive() const {
    return alive;
}

int Client_data::get_pings_missed() const {
    return missed_pings;
}

void Client_data::add_missed_ping() {
    missed_pings = missed_pings + 1;
}

void Client_data::set_socket(int new_socket) {
    socket = new_socket;
}

size_t Client_data::get_game_id() const {
    if(game == nullptr)
        return 0;
    return game->game_id;
}

void Client_data::reset_pings() {
    missed_pings = 0;
}





