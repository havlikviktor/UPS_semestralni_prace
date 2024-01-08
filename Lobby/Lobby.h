#ifndef LOBBY_H
#define LOBBY_H

#include "../Client_data/Client_data.h"
#include "../Game/Game.h"
#include "../Constants.h"
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <memory>

using namespace std;

class Client_data;
class Game;

class Lobby {
public:
    Lobby(string name);
    string get_name() const;
    string get_leader() const;
    vector<string> get_player_names() const;
    vector<int> get_to_notify(const string& skip) const;
    size_t get_inside_count() const;
    int add_player(const shared_ptr<Client_data>& player);
    void leave(Client_data* leaving);

    void show();
    shared_ptr<Game> start_game();
    vector<shared_ptr<Client_data>> in;
private:

    string lobby_name;
    string leader;


    void change_leader();
};


#endif //LOBBY_H
