#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "../Client_data/Client_data.h"
#include "../Constants.h"
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>

using namespace std;

class Client_data;

class Game {

public:
    Game(const string& lobby_name, const vector<shared_ptr<Client_data>>& players);

    /**
     *
     * makes move for given player.
     * @returns number of flied card if valid (card values <MIN_CARD;MAX_CARD>)
     * @returns -1 if player has no more cards to play.
     * @returns -2 if player already flipped his card in this round
     * @returns -3 if player is in rejoining state
     * @returns -4 if there is not such player with this name
     */
    int player_makes_move(const shared_ptr<Client_data>& player);

    /**
     * @return number of player that were in game at when game was started
     */
    int get_players_count();

    /**
     *
     * @return current number of active player. number of players that are still active or in reconnection state
     */
    int get_active_players() const;
    vector<string> get_game_state(const shared_ptr<Client_data>& player);

    /**
     * checks if game is over. evaluated by firstly checking number if active players, then if number of players that
     * has card any card to play is higher that 1
     * @return false if not
     * @returns if game is over of number of active players is less then 2
     */
    bool is_game_over();

    /**
     *
     * @return true if all active players made their move in this round and there is not any player
     * with status reconnection
     */
    bool is_round_over();

    /**
     *
     * @return name of player that has the biggest card on table at the moment
     */
    string find_winner();

    /**
     * for each player sets his card to NO_VALID_CARD if client has no more cards in his deck or NOT_FLIPPED_CARD
     * if he does
     */
    void reset_round();

    /**
     * sets client to temporarily disconnected his card to REJOINING and notify other players in this game
     * @param client that game should wait for
     */
    void wait_for_player(const shared_ptr<Client_data>& client);

    int rejoin(const shared_ptr<Client_data>& client);
    void delete_player(const Client_data* client);

    size_t const game_id;

    /**
     * collect cards from other players and adds them to deck of player with passed name
     * @param winner_name name of winning player
     */
    void collect_cards(const string& winner_name);

private:

    class Player {
    public:
        vector<int> cards;
        int on_table;
        bool flip_in_this_round;

        explicit Player(vector<int> cards): on_table(NOT_FLIPPED_CARD), cards(std::move(cards)), flip_in_this_round(false) {};
        Player() : on_table(NO_VALID_CARD), cards({}), flip_in_this_round(false) {};
    };

    unordered_map<std::string, Player> game_players;
    int active_players;

    static size_t generate_game_id(const string& key);
    void add_player(const shared_ptr<Client_data>& player);
    static vector<int> generate_cards();

};


#endif //SERVER_GAME_H
