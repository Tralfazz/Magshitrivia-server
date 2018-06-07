#include <exception>
#include "TriviaServer.h"



///////---------------------------------- Server methods ----------------------------------///////



TriviaServer::TriviaServer()
{
	//init Database
	this->_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->_socket == INVALID_SOCKET)
	{
		throw std::exception("could not create socket");
	}

}


TriviaServer::~TriviaServer()
{
	std::map<SOCKET, User*>::iterator usersIt;
	std::map<int, Room*>::iterator roomsIt;

	::closesocket(this->_socket);

	for (usersIt = this->_connectedUsers.begin(); usersIt != this->_connectedUsers.end(); usersIt++)
	{
		::closesocket(usersIt->first);
		delete usersIt->second;
	}

	for (roomsIt = this->_roomList.begin(); roomsIt != this->_roomList.end(); roomsIt++)
	{
		::closesocket(roomsIt->first);
		delete roomsIt->second;
	}
}


void TriviaServer::bindAndListen()
{
	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(Protocol::PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;

	if (::bind(this->_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	if (::listen(this->_socket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");

}


void TriviaServer::accept()
{
	SOCKET clientSocket = ::accept(this->_socket, NULL, NULL);

	TRACE("Accepted new client\n")

	if (clientSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	std::thread(&TriviaServer::clientHandler, this, clientSocket).detach();
}


void TriviaServer::server()
{
	this->bindAndListen();
	TRACE("Server is active\nBinded socket and listening on port %d\n" , Protocol::PORT)


	while (true)
	{
		this->accept();
	}
}





RecievedMessage* TriviaServer::buildReciveMessage(SOCKET sc, int msgCode)
{
	std::vector<std::string> msgValues;
	

	switch (msgCode)
	{
		case Protocol::Request::SIGN_IN:
			msgValues =
			{	Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 2)), //username
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 2)) //password 
			};
			
			return new RecievedMessage(sc, msgCode, msgValues);


		case Protocol::Request::SIGN_OUT:
			return new RecievedMessage(sc, msgCode);
			

		case Protocol::Request::SIGN_UP:
			msgValues =
			{
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 2)), //username
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 2)), //password
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 2)), //email 
			};

			return new RecievedMessage(sc, msgCode, msgValues);


		case Protocol::Request::EXISTING_ROOMS:
			return new RecievedMessage(sc, msgCode);
			

		case Protocol::Request::USERS_FROM_ROOM:
			msgValues =
			{
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 4)), //roomID 
			};

			return new RecievedMessage(sc, msgCode, msgValues);


		case Protocol::Request::JOIN_ROOM:
			msgValues =
			{
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 4)), //roomID 
			};

			return new RecievedMessage(sc, msgCode, msgValues);


		case Protocol::Request::LEAVE_ROOM:
			return new RecievedMessage(sc, msgCode);
			

		case Protocol::Request::CREATE_NEW_ROOM:
			msgValues =
			{
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 2)), //room-rame
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 1)), //number of players
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 2)), //number of questions
			};

			return new RecievedMessage(sc, msgCode, msgValues);


		case Protocol::Request::CLOSE_ROOM:
			return new RecievedMessage(sc, msgCode);


		case Protocol::Request::BEGIN_GAME:
			return new RecievedMessage(sc, msgCode);


		case Protocol::Request::GAME_CLIENT_ANSWER:			
			msgValues = 
			{
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 1)), //answer number
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 2))  //time in seconds
			};

			return new RecievedMessage(sc, msgCode, msgValues);


		case Protocol::Request::LEAVE_GAME:
			return new RecievedMessage(sc, msgCode);


		case Protocol::Request::BEST_SCORES:
			return new RecievedMessage(sc, msgCode);


		case Protocol::Request::PERSONAL_MODE:
			return new RecievedMessage(sc, msgCode);

		case Protocol::Request::EXIT_APP:
			return new RecievedMessage(sc, msgCode);
	}
}



void TriviaServer::clientHandler(SOCKET clientSock)
{
	int msgCode = Helper::getMessageTypeCode(clientSock);
	User* currUser = nullptr;


	while (msgCode != Protocol::Request::EXIT_APP && msgCode != 0)
	{
		RecievedMessage* msg = this->buildReciveMessage(clientSock, msgCode);

		switch (msgCode)
		{
			case Protocol::Request::SIGN_IN:
				currUser = this->handleSignin(msg); //assign to user object
				TRACE("Client requested sign in")
				break;

			case Protocol::Request::SIGN_OUT:
				this->handleSignOut(msg);
				TRACE("Client requested sign out")
				break;

			case Protocol::Request::SIGN_UP:
				if(this->handleSignUp(msg)) currUser = msg->getUser();
				TRACE("Client requested sign up")
				break;


			case Protocol::Request::EXISTING_ROOMS:
				this->handleGetRooms(msg);
				TRACE("Client requested all existing rooms")
				break;

			case Protocol::Request::USERS_FROM_ROOM:
				this->handleGetUserInRoom(msg);
				TRACE("Client requested users from room")
				break;

			case Protocol::Request::JOIN_ROOM:
				this->handleJoinRoom(msg); //returns bool
				TRACE("Client requested to join a room")
				break;

			case Protocol::Request::LEAVE_ROOM:
				this->handleLeaveGame(msg); //returns bool
				TRACE("Client requested to leave a room")
				break;

			case Protocol::Request::CREATE_NEW_ROOM:
				this->handleCreateRoom(msg); //returns bool
				TRACE("Client requested to create new room") 
				break;

			case Protocol::Request::CLOSE_ROOM: 
				this->handleCloseRoom(msg); //returns bool
				TRACE("Client requested to close room") 
				break;

			case Protocol::Request::BEGIN_GAME:
				this->handleStartGame(msg);
				TRACE("Client requested to begin game")
				break;

			case Protocol::Request::GAME_CLIENT_ANSWER:
				this->handlePlayerAnswer(msg);
				TRACE("Client sent an answer to the question")
				break;

			case Protocol::Request::LEAVE_GAME:
				this->handleLeaveGame(msg);
				TRACE("Client requested leave game")
				break;

			case Protocol::Request::BEST_SCORES:
				this->handleGetBestScores(msg);
				TRACE("Client requested best scores")
				break;

			case Protocol::Request::PERSONAL_MODE:
				this->handleGetPersonalStatus(msg);
				TRACE("Client requested personal mode")
				break;
		}

		
		if (currUser)
		{

			this->_connectedUsers[clientSock] = currUser;
		}

		msgCode = Helper::getMessageTypeCode(clientSock);
		delete msg;
	}

	::closesocket(clientSock);
}


void TriviaServer::safeDeleteUser(RecievedMessage * msg)
{
}



///////---------------------------------- Handle methods ----------------------------------///////


User* TriviaServer::handleSignin(RecievedMessage* msg)
{
	std::string username = msg->getValues()[0];
	std::string password = msg->getValues()[1];

	std::mutex m;
	std::unique_lock<std::mutex> unique_lock(m);
	std::condition_variable cv;
	

	User usr(username, msg->getSock());
	tmp_user tmp_usr = { &usr , password };


	//cv.wait(unique_lock);


	for (tmp_user& u : this->_tmp_db)
	{
		if ((u._user->getUsername() == tmp_usr._user->getUsername()) && (u._password == tmp_usr._password))
		{
			//TODO check if user is already connected

			Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_IN) + "0"); //Successz
			return u._user;
		}
	}


	//unique_lock.unlock();
	//cv.notify_one();

	Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_IN) + "1"); //Wrong details

	return nullptr;
}


bool TriviaServer::handleSignUp(RecievedMessage* msg)
{
	std::string username = msg->getValues()[0];
	std::string password = msg->getValues()[1];
	std::string email = msg->getValues()[2];

	if (!Validator::isUsernameValid(username))
	{
		Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_UP) + "3");
		return false; //send 1043
	}
		
	if (!Validator::isPasswordValid(password))
	{
		Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_UP) + "1");
		return false; //send 1041
	}

	//this->_connectedUsers.insert(new User())

	User* usr = new User(username, msg->getSock());
	msg->setUser(usr);

	//TODO check if user exists in the db and add it if not
	this->_tmp_db.push_back(tmp_user(usr , password));


	Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_UP) + "0"); //success
	return true;
}



void TriviaServer::handleSignOut(RecievedMessage* msg)
{
	if (this->_connectedUsers[msg->getSock()])
		delete this->_connectedUsers[msg->getSock()];
}


void TriviaServer::handleLeaveGame(RecievedMessage* msg)
{
}


void TriviaServer::handleStartGame(RecievedMessage* msg)
{
}


void TriviaServer::handlePlayerAnswer(RecievedMessage* msg)
{
}


bool TriviaServer::handleCreateRoom(RecievedMessage* msg)
{
	return false;
}


bool TriviaServer::handleCloseRoom(RecievedMessage* msg)
{
	return false;
}


bool TriviaServer::handleJoinRoom(RecievedMessage* msg)
{
	return false;
}


bool TriviaServer::handleLeaveRoom(RecievedMessage* msg)
{
	return false;
}


void TriviaServer::handleGetUserInRoom(RecievedMessage* msg)
{
}


void TriviaServer::handleGetRooms(RecievedMessage* msg)
{
}


void TriviaServer::handleGetBestScores(RecievedMessage* msg)
{
}


void TriviaServer::handleGetPersonalStatus(RecievedMessage* msg)
{
}


void TriviaServer::handleRecievedMessages()
{
}


void TriviaServer::addRecievedMessage(RecievedMessage* msg)
{
}




///////---------------------------------- Getters ----------------------------------///////


Room* TriviaServer::getRoomById(int id)
{
	throw std::exception("Method is not implemented");
	//auto r = std::find(this->_roomList.begin(), this->_roomList.end(), id);
	//return (r != this->_roomList.end()) ? r->second : nullptr;
}


User* TriviaServer::getUserByName(std::string name)
{
	throw std::exception("Method is not implemented");
	//TODO get a vector of names and iterate over it
	//auto usrIt = std::find(std::begin(this->_roomList), std::end(this->_roomList), name);
	//return (usrIt != this->_roomList.end()) ? usrIt->second : nullptr;
}


User* TriviaServer::getUserBySocket(SOCKET sock)
{
	throw std::exception("Method is not implemented");
	//auto usrIt = std::find()
}