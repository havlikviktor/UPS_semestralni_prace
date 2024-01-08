#include "Server_manager.h"

Server_manager::Server_manager(int port, in_addr_t address) {
    struct sockaddr_in my_addr{};

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&my_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = address;


    if (bind(server_socket, (struct sockaddr *) &my_addr,sizeof(struct sockaddr_in)) == 0)
        cout<<"Bind - OK"<<endl;
    else {
        cout<<"Bind - ERR"<<endl;
    }

    create_managers();
}

void Server_manager::create_managers() {
    con_m = Connection_manager();

    cm = Client_manager(con_m);
    lm = Lobby_manager(con_m);
    gm = Game_manager(con_m);
}


[[noreturn]] void Server_manager::start_server(int max_clients) {
        int client_socket, fd;
        int return_value;
        int a2read;
        chrono::high_resolution_clock::time_point last_ping_time = chrono::high_resolution_clock::now();
        struct sockaddr_in peer_addr{};
        socklen_t len_addr = sizeof(peer_addr);
        fd_set client_socks, tests;

        return_value = listen(server_socket, max_clients);

        if (return_value == 0){
            printf("Listen - OK\n");
        } else {
            printf("Listen - ERR\n");
        }

        // vyprazdnime sadu deskriptoru a vlozime server socket
        FD_ZERO( &client_socks );
        FD_SET( server_socket, &client_socks );

        for (;;) {
            struct timeval timeout{std::min(PING_INTERVAL,TIME_TO_RECONNECT),0};
            tests = client_socks;
            // sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
            return_value = select(FD_SETSIZE, &tests, (fd_set *) nullptr,
                                  (fd_set *) nullptr, &timeout);

            if (return_value < 0) {
                cout << "Emptry select" << endl;
            }
            else {
                for (fd = 3; fd < FD_SETSIZE; fd++) {
                    // je dany socket v sade fd ze kterych lze cist ?
                    if (FD_ISSET(fd, &tests)) {
                        // je to server socket ? prijmeme nove spojeni
                        if (fd == server_socket) {
                            client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &len_addr);
                            if (client_socket < 0) {
                                perror("Error in accept");
                                cout << "Error code: " << errno << endl;
                            }
                            FD_SET(client_socket, &client_socks);
                            cout << "Pripojen novy klient a pridan do sady socketu" << endl;
                        }
                            // je to klientsky socket ? prijmem data
                        else {
                            // pocet bajtu co je pripraveno ke cteni
                            ioctl(fd, FIONREAD, &a2read);
                            // mame co cist
                            if (a2read > 0) {
                                handle_request(fd, &client_socks);
                            }
                                // na socketu se stalo neco spatneho
                            else {
                                close(fd);
                                FD_CLR(fd, &client_socks);
                                shared_ptr<Client_data> c = cm.find_client_by_socket(fd);
                                if(c) {
                                    gm.client_leaves(c);
                                    lm.client_leaves(c);
                                    cm.client_leaves(c);
                                }
                            }
                        }
                    }
                }
            }
            if(cm.ping_cycle(last_ping_time))
                kick_inactive(cm.get_inactive());

            disconnect_dead(cm.select_dead_clients(), &client_socks);
        }
}

void Server_manager::handle_request(int client_socket,  fd_set* clients_sockets) {
    list<string> msg = con_m.get_msg_list(client_socket);

    int result = request_handler(msg, client_socket);

    if(result == NOT_REGISTED_CLIENT)
        kill_socket(client_socket, clients_sockets);
}

int Server_manager::request_handler(list<string> request, int client_socket) {
    int result = NO_VALID_REQUEST;
    shared_ptr<Client_data> client;
    string action = request.front();

    if(action == "LOGIN") {
        string login = request.back();
        result = cm.login_client(login, client_socket);

        if(result == IN_LOBBY_STATUS) {
            gm.rejoin_client_to_game(cm.find_client_by_socket(client_socket));
        }

        result = result;
    }

    client = cm.find_client_by_socket(client_socket);

    if(client == nullptr) {
        cout << "WARNING not registered client" << endl;
        if(result == NO_VALID_REQUEST)
            return NOT_REGISTED_CLIENT;
        else
            return result;
    }
    else if(client->get_status() == RECONNECTING_STATUS) {
        cout << "WARNING client sending requests before reconnecting";
        return NOT_REGISTED_CLIENT;
    }

    else if (action == "LOBBY") {
        result = lm.send_lobby(client);
    }

    else if (action == "JOIN") {
        result = lm.join_lobby(client, request.back());
        if(result == 1)
            lm.lobby_change_notify(cm.lobby_change_affected());
    }

    else if (action == "LEAVE") {
        result = lm.leave_lobby(client);
        if(result == 1)
            lm.lobby_change_notify(cm.lobby_change_affected());
    }

    else if (action == "LINFO") {
        result = lm.get_lobby_info(client);
    }

    else if (action == "START") {
        result = gm.start_game(client);

    }

    else if (action == "GAIN") {
        result = gm.get_game_info(client);
    }

    else if (action == "MOVE") {
        result = gm.make_move(client);
        if(result > 0)
            gm.evaluate(client);
    }

    else if (action == "PONG") {
        client->set_alive_state(true);
        result = PING_RESPONCE;
    }


    // nebyla nalezena obsuzna funkce
    if(result == NO_VALID_REQUEST) {
        cout << "function not found for vector : ";
        while (!request.empty()) {
            cout << request.back() << "(" <<  request.back().size() << ")"  << endl;
            request.pop_back();
        }

        con_m.send_message("INVALID_REQUEST", client_socket);
    }

    return result;
}

void Server_manager::add_lobby(const string& lobby_name) {
    lm.add_lobby(lobby_name);
}

void Server_manager::kick_inactive(const std::vector<shared_ptr<Client_data>>& inactive) {
    gm.kick_inactive(inactive);
    lm.kick_inactive(inactive);
    cm.kick_inactive(inactive);
}

void Server_manager::disconnect_dead(const std::vector<shared_ptr<Client_data>>& dead, fd_set* clients_sockets) {
    for(const shared_ptr<Client_data>& client : dead) {
        FD_CLR(client->get_socket(), clients_sockets);
    }
    gm.disconnect_dead(dead);
    lm.disconnect_dead(dead);
    cm.disconnect_dead(dead);
}

void Server_manager::kill_socket(int socket, fd_set* clients_sockets) {
    FD_CLR(socket, clients_sockets);

    shared_ptr<Client_data> c = cm.find_client_by_socket(socket);
    vector<shared_ptr<Client_data>> kill = {c};

    cout << "socket " << socket << " killed - ";
    if(c) {
        gm.disconnect_dead(kill);
        lm.disconnect_dead(kill);
        cm.disconnect_dead(kill);

        cout << "assigned client " << c->get_client_name() << " disconnected" << endl;
    }

    cout << "no client assigned" << endl;
}



