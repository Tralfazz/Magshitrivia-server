#pragma once

#include <WinSock2.h>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <algorithm>

#include "User.h"
#include "DataBase.h"
#include "Room.h"
#include "RecievedMessage.h"
#include "Protocol.h"
#include "Helper.h"


class TriviaServer
{
	private:
		SOCKET _socket;
		std::map<SOCKET, User*> _connectedUsers;
		DataBase _db;
		std::map<int, Room*> _roomList;
		
		std::mutex _mtxRecievedMessages;
		std::queue<RecievedMessage*> _queRcvMessages;

		static int _roomIdSequence;

		
		void bindAndListen();
		void accept();
		
		void clientHandler(SOCKET clientSock);
		void safeDeleteUser(RecievedMessage*);

		User* handleSignin();
		bool hanleSignUp();
		void handleSignOut();

		void handleLeaveGame(RecievedMessage*);
		void handleStartGame(RecievedMessage*);
		void handlePlayerAnswer(RecievedMessage*);

		bool handleCreateRoom(RecievedMessage*);
		bool handleCloseRoom(RecievedMessage*);
		bool handleJoinRoom(RecievedMessage*);
		bool handleLeaveRoom(RecievedMessage*);
		void handleGetUserInRoom(RecievedMessage*);
		void handleGetRooms(RecievedMessage*);


		void handleGetBestScores(RecievedMessage*);
		void handleGetPersonalStatus(RecievedMessage*);

		void handleRecievedMessages();
		void addRecievedMessage(RecievedMessage*);
		RecievedMessage* buildReciveMessage(SOCKET, int);


		User* getUserByName(std::string name);
		User* getUserBySocket(SOCKET sock);
		Room* getRoomById(int id);

	public:

		TriviaServer();
		~TriviaServer();

		void server();

};