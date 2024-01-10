#ifndef LOBBY_MANAGER_H
#define LOBBY_MANAGER_H

#include <memory>
#include <vector>
#include "../Connection_manager/Connection_manager.h"
#include "../../Client_data/Client_data.h"

class Connection_manager;

using namespace std;

class Lobby_manager {

public:
    Lobby_manager();
    explicit Lobby_manager(Connection_manager connection_manager);

    /**
     * sends information about all lobbies to client
     * format <LOBBY_NAME1>;<LEADER_NAME1>;<PLAYERS_COUNT1>|<LOBBY_NAME2>;<LEADER_NAME2>;<PLAYERS_COUNT2>
     * @param client asking for lobbies
     * @return 1
     */
    int send_lobby(const std::shared_ptr<Client_data>& client);

    /**
     * joins client into lobby.
     * @param client client that wants to join lobby
     * @param lobby_name name of lobby client wants to join
     * @returns 1 if client successfully joined lobby
     * @returns -1 if lobby is full
     * @returns -2 if player is already in some other lobby
     * @returns -3 client is already in this lobby
     * @returns -4 if lobby client wants to join wasnt found
     *
     * @see Lobby::add_player()
     */
    int join_lobby(const shared_ptr<Client_data>& client,const string& lobby_name);

    /**
     * removes client form lobby and notify other clients in this lobby about this change
     * @param client leaving lobby
     * @return 1 if client left lobby successfully
     *  @returns -1 if client has no lobby to leave
     *  @returns -2 if client has still on going game in this lobby so he cant leave it
     *
     * @see Client_data::leave_lobby()
     */
    int leave_lobby(const shared_ptr<Client_data>& client);

    /**
     * sends information about clients lobby to this client. first parameter is "member" of "leader" if depends if
     * client is leader of lobby he is in.
     * @param client asking for lobby information
     * @returns 1 if successful
     * @returns -1 if client isnt in any lobby
     */
    int get_lobby_info(const shared_ptr<Client_data>& client);

    /**
     * adds new lobby to lobby list
     * @param lobby_name name of new lobby
     */
    void add_lobby(const string &lobby_name);

    /**
     * notify all clients about change in lobby. calls send_lobby for each client
     * @param clients clients to notify
     *
     * @see send_lobby
     */
    void lobby_change_notify(const std::vector<shared_ptr<Client_data>>& clients);

    void kick_inactive(const vector<shared_ptr<Client_data>> &inactive);

    void client_leaves(const shared_ptr<Client_data> &client);

    void disconnect_dead(const vector<shared_ptr<Client_data>> &dead);

private:
    Connection_manager cm;
    vector<shared_ptr<Lobby>> lobbies;

    static vector<string> create_lobby_format(const shared_ptr<Client_data> &client);

    int send_lobby(const shared_ptr<Client_data> &client, const string& prefix);

    void notify_others(const shared_ptr<Lobby> &lobby, const string &client_name);
};
#endif //LOBBY_MANAGER_H
