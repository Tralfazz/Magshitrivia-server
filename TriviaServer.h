#pragma once

#include <WinSock2.h>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <algorithm>
#include <sstream>

#include "User.h"
#include "DataBase.h"
#include "Room.h"
#include "RecievedMessage.h"
#include "Protocol.h"
#include "Helper.h"
#include "Validator.h"


//tmp user-struct

typedef	struct tmp_user
{
	User* _user;
	std::string _password;

	tmp_user(User* user , std::string password) : _user(user) , _password(password) { }

} tmp_user;



class TriviaServer
{
	private:
		SOCKET _socket;
		std::map<SOCKET, User*> _connectedUsers;
		DataBase _db;
		std::map<int, Room*> _roomList;
		
		std::mutex _qLock;
		std::condition_variable _qCV;
		std::queue<RecievedMessage*> _qRcvMessages;

		static int _roomIdSequence; //roomID counter

		//tmp
		std::vector<tmp_user> _tmp_db;

		
		void bindAndListen();
		void accept();
		
		void clientHandler(SOCKET clientSock);
		void safeDeleteUser(RecievedMessage* msg);

		User* handleSignin(RecievedMessage* msg);
		bool handleSignUp(RecievedMessage* msg);
		void handleSignOut(RecievedMessage*);

		void handleLeaveGame(RecievedMessage* msg);
		void handleStartGame(RecievedMessage* msg);
		void handlePlayerAnswer(RecievedMessage* msg);

		bool handleCreateRoom(RecievedMessage* msg);
		bool handleCloseRoom(RecievedMessage* msg);
		bool handleJoinRoom(RecievedMessage* msg);
		bool handleLeaveRoom(RecievedMessage* msg);
		void handleGetUserInRoom(RecievedMessage* msg);
		void handleGetRooms(RecievedMessage* msg);


		void handleGetBestScores(RecievedMessage* msg);
		void handleGetPersonalStatus(RecievedMessage* msg);

		void handleRecievedMessages();
		void addRecievedMessage(RecievedMessage* msg);
		RecievedMessage* buildReciveMessage(SOCKET sc, int msgCode);


		User* getUserByName(std::string name);
		User* getUserBySocket(SOCKET sock);
		Room* getRoomById(int id);

	public:

		TriviaServer();
		~TriviaServer();

		void server();

};