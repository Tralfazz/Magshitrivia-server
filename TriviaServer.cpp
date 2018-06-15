#include <exception>
#include "TriviaServer.h"



///////---------------------------------- Server methods ----------------------------------///////

int TriviaServer::_roomIdSequence = 0;


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
				Helper::getStringPartFromSocket(sc, 4), //roomID 
			};

			return new RecievedMessage(sc, msgCode, msgValues);


		case Protocol::Request::JOIN_ROOM:
			msgValues =
			{
				Helper::getStringPartFromSocket(sc, 4) //roomID 
			};

			return new RecievedMessage(sc, msgCode, msgValues);


		case Protocol::Request::LEAVE_ROOM:
			return new RecievedMessage(sc, msgCode);
			

		case Protocol::Request::CREATE_NEW_ROOM:
			msgValues =
			{
				Helper::getStringPartFromSocket(sc, Helper::getIntPartFromSocket(sc, 2)), //room-rame
				Helper::getStringPartFromSocket(sc, 1), //number of players
				Helper::getStringPartFromSocket(sc, 2), //number of questions
				Helper::getStringPartFromSocket(sc, 2) //time to answer every question (in seconds)
			};

			return new RecievedMessage(sc, msgCode, msgValues);


		case Protocol::Request::CLOSE_ROOM:
			return new RecievedMessage(sc, msgCode);


		case Protocol::Request::BEGIN_GAME:
			return new RecievedMessage(sc, msgCode);


		case Protocol::Request::GAME_CLIENT_ANSWER:			
			msgValues = 
			{
				Helper::getStringPartFromSocket(sc, 1), //answer number
				Helper::getStringPartFromSocket(sc, 2)  //time in seconds
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

		default:
			throw std::exception("Could not interpret message type code");
	}
}


void TriviaServer::clientHandler(SOCKET clientSock)
{
	int msgCode = Helper::getMessageTypeCode(clientSock);
	User* currUser = nullptr;
	RecievedMessage* msg = nullptr;


	while (msgCode != Protocol::Request::EXIT_APP || msgCode != 0)
	{
		msg = this->buildReciveMessage(clientSock, msgCode);
		msg->setUser(currUser);

		switch (msgCode)
		{
			case Protocol::Request::SIGN_IN:
				currUser = this->handleSignin(msg); //assign to user object
				if (currUser) this->_connectedUsers[clientSock] = currUser;

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

		
		delete msg;
		msgCode = Helper::getMessageTypeCode(clientSock);
	}

	this->safeDeleteUser(msg);
}


void TriviaServer::safeDeleteUser(RecievedMessage* msg)
{
	try 
	{
		this->handleSignOut(msg);
		::closesocket(msg->getSock());
	}
	catch (std::exception& e)
	{
		TRACE("%s", e.what())
	}		
}



///////---------------------------------- Handle methods ----------------------------------///////



/*
 *	This method handles with the sign in request of the user
 *	
 *	@param msg the message which 
 *	@return if the sign in was successful then pointer to the user else null  
*/
User* TriviaServer::handleSignin(RecievedMessage* msg)
{
	std::string username = msg->getValues()[0];
	std::string password = msg->getValues()[1];

	User usr(username, msg->getSock());
	tmp_user tmp_usr = { &usr , password };


	if (_db.isUserAndPassMatch(username , password))
	{
		Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_IN) + "0");
		return new User(username, msg->getSock());
	}

	//for (tmp_user& u : this->_tmp_db)
	//{
	//	if ((u._user->getUsername() == tmp_usr._user->getUsername()) && (u._password == tmp_usr._password))
	//	{
	//		//TODO check if user is already connected
	//
	//		Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_IN) + "0"); //Successz
	//		return u._user;
	//	}
	//}

		
	Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_IN) + "1"); //Wrong details
	return nullptr;
}



/*
*/
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
	//this->_tmp_db.push_back(tmp_user(usr , password));

	if (!_db.isUserExists(username))
	{
		_db.addNewUser(username, password, email);

		Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_UP) + "0"); //success
		return true;
	}

	Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_UP) + "2"); //username already exists
	return false;
}



void TriviaServer::handleSignOut(RecievedMessage* msg)
{
	if (this->_connectedUsers[msg->getSock()])
	{
		this->handleCloseRoom(msg);
		this->handleLeaveRoom(msg);
		this->handleLeaveGame(msg);


		this->_connectedUsers.erase(msg->getSock());
	}
}



void TriviaServer::handleLeaveGame(RecievedMessage* msg)
{
	User* usr = msg->getUser();

	if (usr->leaveGame())
	{
		delete usr->getGame();
	}
}


void TriviaServer::handleStartGame(RecievedMessage* msg)
{
	//std::stringstream msg;

	User* usr = msg->getUser();
	Room* room = usr->getRoom();

	//msg << Protocol::Response:

	if (room)
	{
		try
		{
			Game* newGame = new Game(room->getUsers(), room->getQuestionsNo(), _db);
			usr->setGame(newGame);

			for (User* u : room->getUsers())
			{
				u->setGame(newGame);
				u->clearRoom();
			}

			this->_roomList.erase(room->getId());
			delete room;
			

			newGame->sendFirstQuestion();			
		}
		catch (const std::exception& e)
		{
			TRACE("Error in TriviaServer::HandleStartGame: %s" , e.what())
		}
	}
}



void TriviaServer::handlePlayerAnswer(RecievedMessage* msg)
{
	User* currUser = msg->getUser();
	Game* game = currUser->getGame();

	int ansNum = std::stoi(msg->getValues()[0]);
	int timeInSeconds = std::stoi(msg->getValues()[1]);

	if (game)
	{
		game->handleAnswerFromUser(currUser, ansNum, timeInSeconds);
	}
	else
	{
		//TODO delete game from memory (but its already null?!?!?)
	}
}


bool TriviaServer::handleCreateRoom(RecievedMessage* msg)
{
	User* usr = msg->getUser();

	if (usr)
	{
		std::string roomName = msg->getValues()[0];
		int numOfPlayers = std::stoi(msg->getValues()[1]);
		int numOfQuestions = std::stoi(msg->getValues()[2]);
		int timeInSeconds = std::stoi(msg->getValues()[3]);

		_roomIdSequence++;

		if (usr->createRoom(_roomIdSequence, roomName, numOfPlayers, numOfQuestions, timeInSeconds))
		{
			this->_roomList[_roomIdSequence] = usr->getRoom();
			return true;
		}
	}

	return false;
}


bool TriviaServer::handleCloseRoom(RecievedMessage* msg)
{
	User* usr = msg->getUser();
	Room* room = usr->getRoom();

	if (room)
	{
		int roomId = room->getId();

		if (usr->closeRoom() != -1) //successful operation
		{
			this->_roomList.erase(roomId);
			return true;
		}
	}

	return false;
}


bool TriviaServer::handleJoinRoom(RecievedMessage* msg)
{
	User* usr = msg->getUser();

	if (usr)
	{
		int roomId = std::stoi(msg->getValues()[0]);
		Room* requestedRoom = this->getRoomById(roomId);

		if (requestedRoom)
		{
			usr->joinRoom(requestedRoom);
			return true;
		}
		else
			Helper::sendData(usr->getSocket(), Protocol::Request::JOIN_ROOM + "2"); //room is not found
		
	}

	return false;
}


bool TriviaServer::handleLeaveRoom(RecievedMessage* msg)
{
	User* usr = msg->getUser();
	
	if (!usr) return false;
	if (!usr->getRoom()) return false;

	usr->leaveRoom();

	return true;
}


void TriviaServer::handleGetUserInRoom(RecievedMessage* msg)
{
	Room* r = this->getRoomById(std::stoi(msg->getValues()[0]));
	if(r) Helper::sendData(msg->getSock(), r->getUsersListMessage());
}



void TriviaServer::handleGetRooms(RecievedMessage* msg)
{
	std::stringstream roomsMsg;
	roomsMsg << Protocol::Response::EXISTING_ROOMS << Helper::getPaddedNumber(this->_roomList.size() , 4);


	for (std::pair<int, Room*> i : this->_roomList)
	{
		std::string roomId = Helper::getPaddedNumber(i.first , 4);
		std::string roomName = i.second->getName();
		std::string roomStrSize = Helper::getPaddedNumber(roomName.length(), 2);

		roomsMsg << roomId <<  roomStrSize << roomName;
	}

	Helper::sendData(msg->getSock(), roomsMsg.str());
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
	std::map<int, Room*>::iterator it;

	for (it = this->_roomList.begin(); it != this->_roomList.end(); it++)
	{
		if (it->first == id)
		{
			return it->second;
		}
	}

	return nullptr;
}


User* TriviaServer::getUserByName(std::string name)
{
	std::map<SOCKET, User*>::iterator it;

	for (it = this->_connectedUsers.begin(); it != this->_connectedUsers.end(); it++)
	{
		if (it->second->getUsername() == name)
		{
			return it->second;
		}
	}

	return nullptr;
}


User* TriviaServer::getUserBySocket(SOCKET sock)
{
	std::map<SOCKET, User*>::iterator it;

	for (it = this->_connectedUsers.begin(); it != this->_connectedUsers.end(); it++)
	{
		if (it->second->getSocket() == sock)
		{
			return it->second;
		}
	}

	return nullptr;
}