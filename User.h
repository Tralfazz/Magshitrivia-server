#pragma once

#include <WinSock2.h>
#include <string>
#include "Helper.h"
#include "Room.h"
#include "Game.h"
#include "Protocol.h"


class Room;
class Game;

class User
{
	private:
		std::string _username;
		Room* _currRoom;
		Game* _currGame;
		SOCKET _sock;
	
	public:
		User(std::string username, SOCKET sock);

		void send(std::string msg);

		std::string getUsername();
		SOCKET getSocket();
		Room* getRoom();
		Game* getGame();

		void setGame(Game* gm);
		void clearGame();

		bool createRoom(int roomId, std::string roomName, int maxUsers, int questionsNo, int questionTime);
		bool joinRoom(Room* newRoom);
		void leaveRoom();
		int closeRoom();
		bool leaveGame();
		void clearRoom();
};