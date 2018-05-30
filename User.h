#pragma once

#include <WinSock2.h>
#include <string>
#include "Room.h"
#include "Game.h"


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
		void clearRoom();
		bool createRoom(int, std::string, int, int, int);
		bool joinRoom(Room*);
		void leaveRoom();
		int closeRomm();
		bool leaveGame();
};