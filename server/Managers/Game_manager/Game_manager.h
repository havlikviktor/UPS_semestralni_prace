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

    /**
     * rejoins client into game and notify other clients in this game about this
     * @param client that is rejoining into game
     *
     * @see Game::rejoin()
     */
    void rejoin_client_to_game(const shared_ptr<Client_data>& client);

    /**
     *  starts game and notify other users in this game about it
     * @param client client that is asking for game to start
     * @return 1 if game started
     * @returns -1 if client is not in any lobby for which he can start the game
     * @returns -2 if client is not leader of lobby
     * @returns -3 if lobby has less then 2 clients in it
     * @returns -4 if client is still in some other game so new game cant be started (should not happened)
     *
     * @see can_game_begin()
     */
    int start_game(const shared_ptr<Client_data>& client);

    /**
     * makes move for given client. if move is valid notify other player in game about this move and evaluates game
     * @param client that makes his move
     *
     * @returns number of flied card if valid (card values <MIN_CARD;MAX_CARD>)
     * @returns -1 if player has no more cards to play.
     * @returns -2 if player already flipped his card in this round
     * @returns -3 if player is in rejoining state
     * @returns -4 if there is not such player with this name
     * @returns -5 if client is not in any game
     *
     * @see Game::player_makes_move()
     * @see notify_others()
     * @see evaluate()
     */
    int make_move(const shared_ptr<Client_data>& client);

    /**
     * evaluates game of given client. checks if round is over and if so sends notification about this to other clients
     * in this game. finds winner, collects cards for him and resets round. then checks if game is over and if so
     * sends notification about this to other clients.
     * @param client which game should be evaluated
     *
     * @see Game::find_winner()
     * @see Game::reset_round()
     * @see Game::collect_cards()
     * @see notify_others()
     */
    void evaluate(const shared_ptr<Client_data>& client);

    /**
     * sends information about clients game in format <player_name1>;<player_cards1>|<player_name2>;<player_cards2>|...
     * where client asking for info in sent as first
     * @param client asking for information about game
     * @return -1 if client isnt in any game
     * @returns 1 if info is sent successfully
     */
    int get_game_info(const shared_ptr<Client_data>& client);

    /**
     * calls client_leaves for every client
     * @param inactive vector of clients to temporarily disconnect
     *
     * @see client_leaves()
     */
    void kick_inactive(const vector<shared_ptr<Client_data>> &inactive);

    /**
     * if client has on going game, this function calls wait_for_player() on this game
     * and notify other clients in this game about client distraction
     * @param inactive client that is temporarily leaving server
     *
     * @see Game::wait_for_player()
     * @see notify_others()
     */
    void client_leaves(const shared_ptr<Client_data> &inactive);

    /**
     * calls leave_game() for each client in this vector, if client has on going game notify other clients in this game
     * about client permanent distraction, after that evaluates this game.
     * @param dead vector of clients that are considered dead and are removed for server
     *
     * @see Client_data::leave_game()
     * @see notify_others()
     * @see evaluate()
     */
    void disconnect_dead(const vector<shared_ptr<Client_data>> &dead);

private:
    Connection_manager cm;

    void notify_others(const string& notification, const shared_ptr<Client_data> &client);

    static void delete_game(const shared_ptr<Client_data>& ptr);

    void notify_others(const shared_ptr<Client_data> &client, const string &prefix, const string &flag);
};

#endif //GAME_MANAGER_H
