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
    /**
     * @return name of lobby
     */
    string get_name() const;
    /**
     *
     * @return name of leader
     * @returns NO LEADER if there is on any client in lobby
     */
    string get_leader() const;
    /**
     *
     * @returns vector of names of clients in this lobby. name of leader is push as first name
     */
    vector<string> get_player_names() const;

    /**
     *
     * @return number of clients inside this lobby
     */
    size_t get_inside_count() const;
    /**
    * adds player into this lobby.
     * @Returns 1 if added successfully
    * @returns -1 if lobby is full
    * @returns -2 if player is already in some other lobby
    * @returns -3 client is already in this lobby
 */
    int add_player(const shared_ptr<Client_data>& player);

    /**
     * if there is client in such name inside lobby removes him
     * if client was leader of this lobby calls change_leader()
     * @param leaving client that is leaving lobby
     *
     * @see change_leader()
     */
    void leave(Client_data* leaving);

    void show();
    shared_ptr<Game> start_game();
    vector<shared_ptr<Client_data>> in;
private:

    string lobby_name;
    string leader;

    /**
     * sets new leader for this lobby. lobby is not empty sets leader as first client in this lobby
     * otherwise sets leader to "NO LEADER"
     */
    void change_leader();
};


#endif //LOBBY_H
