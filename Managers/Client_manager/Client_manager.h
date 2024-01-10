#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <list>
#include "../Connection_manager/Connection_manager.h"
#include "../../Client_data/Client_data.h"

class Connection_manager;
class Client_data;

using namespace std;

class Client_manager {
public:
    Client_manager();
    explicit Client_manager(Connection_manager connection_manager);

    /**
 * takes login of client and created object of Client_data which is added into client vector.
 *
 *  @param login login client trys to use
 *  @param client_socket socket client is communicating through
 *  @returns 0 if client is added successfully
 *  @returns 1 if client reconnected successfully
 *  @returns 2 if client reconnected successfully into game
 *  @returns -1 if login is already taken by other user
 *  @returns -2 if client name doesnt match criteria
 */
    int login_client(const string& login,int  client_socket);
    /**
     * finds associated client object to given socket
     * @param socket  socket client is communicating through
     * @return shared pointer at client object or nullptr if client wasnt found (usually happens when client is not
     * registered yet)
     */
    shared_ptr<Client_data> find_client_by_socket(int socket);

    /**
     * finds associated client object to given client login
     *
     * @param name that client is using as his unique login
     * @returns shared pointer at client object or nullptr if client wasnt found (usually happens when client is not
     * registered yet)
     */
    shared_ptr<Client_data> find_client_by_name(const string &name);
    /**
     * sets client status to RECONNECTING_STATUS and marks the time when client left the server
     *
     * @param client that leaves server
     */
    void client_leaves(const shared_ptr<Client_data>& client);

    /**
     * creates ping cycle that pings all active clients if time since last ping is greater that PING_INTERVAL and
     * rests @param last_ping_time to actual time
     * @param last_ping_time time of last ping cycle
     * @return true if ping cycle happened, false if not (means interval between now and time since last ping
     * is smaller then PING_INTERVAL)
     *
     * @see ping_all()
     * @see reset_pings()
     */
    bool ping_cycle(chrono::high_resolution_clock::time_point& last_ping_time);


    /**
     * selects all clients that has status RECONNECTING_STATUS and time since they left the server is longer than TIME_TO_RECONNECT
     * @return vector of clients that's are considered as dead and should be deleted from server
     */
    vector<shared_ptr<Client_data>> select_dead_clients();

    /**
     * selects all clients that are affected by changes when other clients leaves of joins lobbies (typically players
     * that has status LOGIN_STATUS to see change in lobby players count or change of leader)
     * @return vector of clients that should be notified about this change
     */
    std::vector<shared_ptr<Client_data>> lobby_change_affected();

    /**
     * selects all clients that are in active state and count of their missed pings is greater then MAX_MISSED_PINGS
     * @return vector of clients that should be marked as RECONNECTING_STATUS
     */
    std::vector<std::shared_ptr<Client_data>> get_inactive();

    /**
     * marks all given clients with RECONNECTING_STATUS
     * @param inactive vector of clients to set as RECONNECTING_STATUS. method client_leaves is called for each client here
     *
     * @see client_leaves()
     */
    void kick_inactive(const vector<shared_ptr<Client_data>> &inactive);

    /**
     * removes all given clients from server. user_disconnect is called for each client
     * @param inactive inactive clients that are about to be removed from server
     *
     * @see user_disconnect()
     */
    void disconnect_dead(const vector<shared_ptr<Client_data>> &inactive);

private:
    Connection_manager cm;
    list<shared_ptr<Client_data>> clients;

    /**
     *
     * @param client instance of client that is trying to reconnect to server by calling login again
     * @param client_socket socket client communicates through
     * @return 1 if client reconnected successfully
     * @returns 2 if client reconnected successfully and has on going game
     * @returns -1 if client is still connected and there is no need to reconnect (also typically can happen when other
     * client is calling login with name that is already taken by this client instance)
     */
    int try_reconnect(const shared_ptr<Client_data>& client, int client_socket);

    /**
     * sends "PING" message to all active clients in other words to clients not marked as INVALID_STATUS or RECONNECTING_STATUS
     */
    void ping_all();

    /**
     * evaluates if client sent ping responce in recent ping cycle by checking its alive_status. if alive_status is set
     * to true changes it to false for next cycle if set to false, increases his missed ping counter
     */
    void reset_pings();

    /**
     * disconnects client from server by removing him from list of clients
     * @param client that is about to be disconnected
     */
    void user_disconnect(shared_ptr<Client_data>& client);
};

#endif// CLIENT_MANAGER_H
