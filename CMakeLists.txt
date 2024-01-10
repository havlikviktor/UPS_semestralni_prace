cmake_minimum_required(VERSION 3.18.4)
project(untitled4)

set(CMAKE_CXX_STANDARD 17)

add_executable(server_ups main.cpp Managers/Server_manager/Server_manager.h Managers/Server_manager/Server_manager.cpp Constants.h Managers/Connection_manager/Connection_manager.cpp Managers/Connection_manager/Connection_manager.h Managers/Connection_manager/Connection_manager.h Managers/Client_manager/Client_manager.cpp Managers/Client_manager/Client_manager.h Managers/Lobby_manager/Lobby_manager.cpp Managers/Lobby_manager/Lobby_manager.h Client_data/Client_data.cpp Client_data/Client_data.h Lobby/Lobby.cpp Lobby/Lobby.h Game/Game.cpp Game/Game.h Managers/Game_manager/Game_manager.cpp Managers/Game_manager/Game_manager.h)
