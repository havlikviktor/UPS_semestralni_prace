#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

#include <string>
#include <chrono>
#include "../Lobby/Lobby.h"
#include "../Game/Game.h"
#include "../Constants.h"

using namespace std;

class Lobby;
class Game;

class Client_data {
public:
    Client_data();

    Client_data(int socket, string client_name);

    int get_socket() const;
    void set_socket(int socket);
    void set_status(int status);
    int get_status() const;
    string get_client_name() const;
    size_t get_game_id() const;
    shared_ptr<Lobby> get_lobby() const;

    void set_lobby(shared_ptr<Lobby>);
    int leave_lobby();
    void leave_game();
    void set_alive_state(bool state);
    bool is_alive() const;
    int get_pings_missed() const;
    void add_missed_ping();
    void reset_pings();



    bool operator==(const Client_data& other) const;

    void show();

    shared_ptr<Game> game;
    shared_ptr<Lobby> lobby_in;
    string client_name;
    chrono::high_resolution_clock::time_point leave_time;

private:
    bool alive;
    int socket, status, missed_pings;



};

#endif // CLIENT_DATA_H