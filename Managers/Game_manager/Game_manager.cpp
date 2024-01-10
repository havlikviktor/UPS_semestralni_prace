
#include "Game_manager.h"

Game_manager::Game_manager() = default;
Game_manager::Game_manager(Connection_manager connection_manager): cm(connection_manager) {
}

void Game_manager::rejoin_client_to_game(const shared_ptr<Client_data>& client) {
    shared_ptr<Game> client_game = client->game;

    if(client_game == nullptr) {
        cout << "client " << client->get_client_name() << " has no game to join" << endl;
        return;
    }

    if(client_game->rejoin(client)) {
        cout<< "game rejoined" << endl;
        notify_others(client,"UPDATE","");
    }
}

int can_game_begin(const shared_ptr<Client_data>& client) {
    shared_ptr<Lobby> lobby = client->get_lobby();
    if(lobby == nullptr)
        return -1;

    if(lobby->get_leader() != client->get_client_name())
        return -2;

    if(lobby->get_inside_count() < 2) {
        return -3;
    }

    if(client->game != nullptr)
        return -4;

    return 1;
}

int Game_manager::start_game(const shared_ptr<Client_data>& client) {

    int result = can_game_begin(client);

    if(result != 1) {
        cm.send_message("ERR " + to_string(result), client->get_socket());
    }
    else {
        shared_ptr<Game> game = client->get_lobby()->start_game();
        cm.send_message("OK " + to_string(game->get_players_count()), client->get_socket());
        notify_others("START " + to_string(game->get_players_count()), client);
    }

    return result;
}

void Game_manager::notify_others(const string& notification, const shared_ptr<Client_data> &client) {
    vector<shared_ptr<Client_data>> players = client->get_lobby()->in;
    size_t game_id = client->game->game_id;

    for(const shared_ptr<Client_data>& player : players) {
        if(player != client) {

        if(player->game->game_id == game_id)
            cm.send_message(notification, player->get_socket());
        else
            cout << "not matching game_id for " << client->get_client_name() << " in lobby " <<client->get_lobby()->get_name() << endl;
        }

    }
}

void Game_manager::notify_others(const shared_ptr<Client_data> &client, const string& prefix, const string& flag) {
    vector<shared_ptr<Client_data>> players = client->get_lobby()->in;
    size_t game_id = client->game->game_id;

    for(const shared_ptr<Client_data>& player : players) {
        if(player != client && player->game != nullptr) {
            if(player->game->game_id == game_id) {
                vector<string> info = player->game->get_game_state(player);
                if(!flag.empty())
                    info.emplace_back(flag);
                info.emplace_back(to_string(IN_GAME_STATUS));
                info.emplace_back(prefix);
                cm.send_message(info, player->get_socket());
            }
            else
                cout << "not matching game_id for " << client->get_client_name() << " in lobby " <<client->get_lobby()->get_name() << endl;
        }

    }
}

int Game_manager::make_move(const shared_ptr<Client_data>& client) {
    shared_ptr<Game> game = client->game;
    if(game == nullptr) {
        cm.send_message("ERR -5", client->get_socket());
        return -5;
    }

    int result = game->player_makes_move(client);

    if(result < 1)
        cm.send_message("ERR " + to_string(result), client->get_socket());
    else {
        cm.send_message("OK " + to_string(result), client->get_socket());
        vector<string> info = game->get_game_state(client);
        info.emplace_back(to_string(IN_GAME_STATUS));
        info.emplace_back("UPDATE");
        notify_others(client,"UPDATE","");
    }

    return result;
}

void Game_manager::evaluate(const shared_ptr<Client_data>& client) {
    shared_ptr<Game> game = client->game;
    if(game == nullptr || !game->is_round_over()) {
        return;
    }

    string winner = game->find_winner();
    game->collect_cards(winner);
    game->reset_round();
    vector<string> info = game->get_game_state(client);
    info.emplace_back(winner);
    info.emplace_back(to_string(IN_GAME_STATUS));
    if(game->is_game_over()) {
        info.emplace_back("GAMER");
    }
    else
        info.emplace_back("WINNER");

    cm.send_message(info, client->get_socket());
    notify_others(client,info.back(),winner);

    if(game->is_game_over()) {
        cout << "DELETING GAME FOR ALL PLAYERS" << endl;
        delete_game(client);
    }
}

int Game_manager::get_game_info(const shared_ptr<Client_data>& client) {
    shared_ptr<Game> game = client->game;
    if(game == nullptr) {
        cm.send_message("ERR -1", client->get_socket());
        return -1;
    }

    vector<string> info = game->get_game_state(client);

    info.emplace_back("1");
    info.emplace_back("OK");
    cm.send_message(info, client->get_socket());

    return 1;
}

void Game_manager::delete_game(const shared_ptr<Client_data>& client) {
    vector<shared_ptr<Client_data>> clients = client->get_lobby()->in;

    for(const shared_ptr<Client_data>& player : clients) {
        player->leave_game();
    }
}

void Game_manager::client_leaves(const shared_ptr<Client_data> &client) {
    if(client == nullptr || client->game == nullptr)
        return;

    client->game->wait_for_player(client);
    notify_others(client, "UPDATE", "");
}

void Game_manager::kick_inactive(const vector<shared_ptr<Client_data>> &inactive) {
    for(const shared_ptr<Client_data>& client : inactive) {
        client_leaves(client);
    }
}

void Game_manager::disconnect_dead(const vector<shared_ptr<Client_data>> &dead) {
    for(const shared_ptr<Client_data>& client : dead) {
        if(client->game == nullptr)
            continue;
        //TODO: prasarničky
        shared_ptr<Client_data> copy = make_shared<Client_data>();
        copy->set_lobby(client->get_lobby());
        copy->game = client->game;

        client->leave_game();
        notify_others(copy, "UPDATE", "");
        evaluate(copy);
    }
}


