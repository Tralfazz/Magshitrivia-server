#pragma once

#include <WinSock2.h>
#include <string>
#include "Helper.h"
#include "Room.h"
#include "Game.h"
#include "Protocol.h"


class Room;

class User
{
	private:
		std::string _username;
		Room* _currRoom;
		Game* _currGame;
		SOCKET _sock;
	
	public:
		User(std::string, SOCKET);
		void send(std::string);
		std::string getUsername();
		SOCKET getSocket();
		Room* getRoom();
		Game* getGame();
		void setGame(Game*);
		void clearGame();
		bool createRoom(int, std::string, int, int, int);
		bool joinRoom(Room*);
		void leaveRoom();
		int closeRoom();
		bool leaveGame();
};