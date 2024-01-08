#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <climits>

////////////////// server //////////////////////
#define MAX_CLIENTS 10

#define NO_VALID_REQUEST INT_MIN
#define NOT_REGISTED_CLIENT INT_MAX
#define MSG_SEPARATOR '|'

#define PING_RESPONCE 69
#define PING_INTERVAL 1000             //[s]
#define MAX_MISSED_PINGS 3
#define TIME_TO_RECONNECT 5          //[s]

///////////////// Client //////////////////////
#define NAME_MAX_LENGTH 20
#define NAME_MIN_LENGTH 2

#define INVALID_STATUS (-2)
#define RECONNECTING_STATUS (-1)
#define LOGIN_STATUS 1
#define IN_LOBBY_STATUS 2
#define IN_GAME_STATUS 3

////////////// Lobby /////////////////////////
#define MAX_IN_LOBBY 4

///////////// Game //////////////////////////

#define NO_VALID_NAME "no winner"

#define MIN_CARD 1
#define MAX_CARD 8
#define CARDS_AT_BEGINNING 2

#define NOT_FLIPPED_CARD 0
#define NO_VALID_CARD (-1)

#define REJOINING (-2)
#define DISCONNECTED (-3)



#endif //CONSTANTS_H
