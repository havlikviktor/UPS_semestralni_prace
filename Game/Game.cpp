#include <random>
#include "Game.h"


Game::Game(const string& lobby_name,const vector<shared_ptr<Client_data>>& players) : game_id(generate_game_id(lobby_name)) {
    for(const shared_ptr<Client_data>& p : players) {
        add_player(p);
    }

    active_players = (int) players.size();
}

size_t Game::generate_game_id(const string& key) {
    hash<string> hash_function;

    return hash_function(key);
}

void Game::add_player(const shared_ptr<Client_data>& player) {
    game_players[player->get_client_name()] = Player(generate_cards());
}

vector<int> Game::generate_cards() {
    vector<int> pack;
    random_device rd;

    mt19937 mt(rd());
    uniform_int_distribution<int> dist(MIN_CARD, MAX_CARD);

    for(int i = 0; i < CARDS_AT_BEGINNING;i++) {
        pack.push_back(dist(mt));
    }

    return pack;
}

vector<string> Game::get_game_state(const shared_ptr<Client_data>& player) {
    vector<string> state;
    string questioning;

    for (const auto& pair : game_players) {
        string s = pair.first + ";" + to_string(pair.second.on_table);
        if(pair.first == player->get_client_name()) {
            questioning = s;
            continue;
        }
        state.push_back(s);
    }

    state.push_back(questioning);
    return state;
}
/*
 * makes move for given player. returns number of flipped card or:
 *  -1 if player has no more cards to play.
 *  -2 if player already flipped his card in this round
 *  -3 if player is in rejoining state
 *  -4 if there is not such player with this name
 */
int Game::player_makes_move(const shared_ptr<Client_data>& player) {
    for (auto & game_player : game_players) {

        if(game_player.first == player->get_client_name()) {
            if(game_player.second.on_table == REJOINING)
                return -3;

            if(game_player.second.cards.empty()) {
                game_player.second.on_table = NO_VALID_CARD;
                return -1;
            }
            if(!game_player.second.flip_in_this_round) {
                game_player.second.on_table = game_player.second.cards.back();
                game_player.second.flip_in_this_round = true;
                game_player.second.cards.pop_back();

                return game_player.second.on_table;
            }
            else
                return -2;
        }
    }
    return -4;
}

int Game::get_players_count() {
    return (int) game_players.size();
}

bool Game::is_game_over() {
    if(active_players == 1)
        return true;

    int players_left = 0;
    for (auto & game_player : game_players) {
        if(game_player.second.on_table > -1)
            players_left++;
    }

    return players_left < 2;
}

bool Game::is_round_over() {
    if(active_players == 1)
        return true;

    for (auto & game_player : game_players) {
        if((!game_player.second.flip_in_this_round && !game_player.second.cards.empty()) || game_player.second.on_table == REJOINING)
            return false;
    }

    return true;
}

void Game::reset_round() {
    for (auto & game_player : game_players) {
        if(game_player.second.on_table == DISCONNECTED)
            continue;

        game_player.second.flip_in_this_round = false;

        if(!game_player.second.cards.empty())
            game_player.second.on_table = NOT_FLIPPED_CARD;

        else if(game_player.second.on_table != DISCONNECTED && game_player.second.on_table != REJOINING)
            game_player.second.on_table = NO_VALID_CARD;
    }
}

void Game::collect_cards(const string& winner_name) {

    auto winner = game_players.find(winner_name);
    for (auto & game_player : game_players) {
        winner->second.cards.push_back(game_player.second.on_table);
        cout << winner->first << " collecting: " << game_player.second.on_table << endl;
    }
}


string Game::find_winner() {

    string winner = NO_VALID_NAME;
    int card_value = NO_VALID_CARD;
    int pack_size = -1;

    for (const auto &game_player : game_players) {
        if(game_player.second.on_table > card_value ||
           (game_player.second.on_table == card_value && game_player.second.cards.size() > pack_size))
        {
            cout << game_player.first << " with " << game_player.second.on_table <<" is better then "
                 << winner << "with: " << card_value << endl;

            winner = game_player.first;
            card_value =  game_player.second.on_table;
            pack_size = (int) game_player.second.cards.size();
        }
    }

    cout << winner << " is winner" << endl;
    return winner;
}

void Game::delete_player(const Client_data* client) {
    if(client->get_game_id() != game_id)
        return;

    for (auto &game_player : game_players) {
        if(game_player.first == client->get_client_name()) {
            game_player.second.on_table = DISCONNECTED;
            game_player.second.flip_in_this_round = true;
            game_player.second.cards.clear();
            active_players--;
        }
    }
}

int Game::get_active_players() const {
    return active_players;
}

void Game::wait_for_player(const shared_ptr<Client_data> &client) {
    if(client->get_game_id() != game_id)
        return;

    for (auto &game_player : game_players) {
        if(game_player.first == client->get_client_name()) {
            game_player.second.cards.push_back(game_player.second.on_table);
            game_player.second.on_table = REJOINING;
        }
    }
}

int Game::rejoin(const shared_ptr<Client_data>& client) {
    for (auto &game_player : game_players) {

        if(game_player.first == client->get_client_name()) {
            game_player.second.on_table = game_player.second.cards.back();
            game_player.second.cards.pop_back();
            return 1;
        }
    }

    return 0;
}
