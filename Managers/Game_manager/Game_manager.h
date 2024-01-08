#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <vector>
#include <list>
#include "../Connection_manager/Connection_manager.h"
#include "../../Client_data/Client_data.h"
#include "../../Game/Game.h"



class Connection_manager;
class Client_data;
class Game;

using namespace std;

class Game_manager {
public:
    Game_manager();
    explicit Game_manager(Connection_manager connection_manager);


    void rejoin_client_to_game(const shared_ptr<Client_data>& client);

    int start_game(const shared_ptr<Client_data>& client);

    int make_move(const shared_ptr<Client_data>& client);

    void evaluate(const shared_ptr<Client_data>& client);

    int get_game_info(const shared_ptr<Client_data>& client);

    void kick_inactive(const vector<shared_ptr<Client_data>> &inactive);

    void client_leaves(const shared_ptr<Client_data> &inactive);

    void disconnect_dead(const vector<shared_ptr<Client_data>> &dead);

private:
    Connection_manager cm;

    void notify_others(const string& notification, const shared_ptr<Client_data> &client);

    static void delete_game(const shared_ptr<Client_data>& ptr);

    void notify_others(const shared_ptr<Client_data> &client, const string &prefix, const string &flag);
};

#endif //GAME_MANAGER_H
