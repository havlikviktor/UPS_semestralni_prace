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

    int player_makes_move(const shared_ptr<Client_data>& player);
    int get_players_count();
    int get_active_players() const;
    vector<string> get_game_state(const shared_ptr<Client_data>& player);

    bool is_game_over();
    bool is_round_over();
    string find_winner();
    void reset_round();

    void wait_for_player(const shared_ptr<Client_data>& client);
    int rejoin(const shared_ptr<Client_data>& client);
    void delete_player(const Client_data* client);

    size_t const game_id;

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
