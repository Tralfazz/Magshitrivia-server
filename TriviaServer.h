#pragma once

#include <WinSock2.h>
#include <map>
#include <mutex>
#include <queue>

#include "User.h"
#include "DataBase.h"
#include "Room.h"
#include "RecivedMessage.h"


class TriviaServer
{
	private:
		SOCKET _socket;
		std::map<SOCKET, User*> _connectedUsers;
		DataBase _db;
		std::map<int, Room*> _roomList;
		
		std::mutex _mtxRecivedMessages;
		std::queue<RecivedMessage*> _queRcvMessages;

		static int _roomIdSequence;

		
		void bindAndListen();
		void accept();
		
		void clientHandler(SOCKET);
		void safeDeleteUser(RecivedMessage*);

		User* handleSignin();
		bool hanleSignUp();
		void handleSignOut();

		void handleLeaveGame(RecivedMessage*);
		void handleStartGame(RecivedMessage*);
		void handlePlayerAnswer(RecivedMessage*);

		bool handleCreateRoom(RecivedMessage*);
		bool handleCloseRoom(RecivedMessage*);
		bool handleJoinRoom(RecivedMessage*);
		bool handleLeaveRoom(RecivedMessage*);
		void handleGetUserInRoom(RecivedMessage*);
		void handleGetRooms(RecivedMessage*);


		void handleGetBestScores(RecivedMessage*);
		void handleGetPersonalStatus(RecivedMessage*);

		void handleRecivedMessages();
		void addRecivedMessage(RecivedMessage*);
		RecivedMessage* buildReciveMessage(SOCKET, int);


		User* getUserByName(std::string name);
		User* getUserBySocket(SOCKET);
		Room* getRoomById(int id);

	public:

		TriviaServer();
		~TriviaServer();

		void server();

};