#include <exception>
#include "TriviaServer.h"



///////---------------------------------- Server methods ----------------------------------///////

int TriviaServer::_roomIdSequence = 0;


/*
*	TriviaServer c'tor
*	
*	Initializes the listening socket
*/
TriviaServer::TriviaServer()
{
	//init Database
	this->_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->_socket == INVALID_SOCKET)
	{
		throw std::exception("could not create socket");
	}
}


/*
*	TriviaServer d'tor
*
*	closes the listening socket, deletes all of the users and rooms
*/
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


/*
*	Bindes and listens with the listening socket
*/
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


/*
*	Accepts a new client and creats its clinet handler thread
*/
void TriviaServer::accept()
{
	SOCKET clientSocket = ::accept(this->_socket, NULL, NULL);

	TRACE("Accepted new client\n")

	if (clientSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	std::thread(&TriviaServer::clientHandler, this, clientSocket).detach();
}


/*
*	Sets up the server
*
*	(Binds and listens to new clients and creates the handleReivedMessages thread)
*/
void TriviaServer::server()
{
	this->bindAndListen();
	TRACE("Server is active\nBinded socket and listening on port %d\n", Protocol::PORT)

	std::thread(&TriviaServer::handleRecievedMessages, this).detach(); // message queue handler
	
	while (true)
	{
		this->accept();
	}
}


/*
*	Builds up a new RecivedMessage object
*
*	@param sc the socket of the client
*	@param msgCode the message code of the message
*
*	@return a new RecivedMessage object
*/
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


/*
*	The client handler method (pushes messages to the queue)
*
*	@param clientSock the socket of the client
*/
void TriviaServer::clientHandler(SOCKET clientSock)
{
	RecievedMessage* msg = nullptr;
	User* currUsr = nullptr;
	std::map<SOCKET, User*>::iterator iterUsr;

	try
	{
		int msgCode = Helper::getMessageTypeCode(clientSock);

		while (msgCode != Protocol::Request::EXIT_APP || msgCode != 0)
		{
			iterUsr = _connectedUsers.find(clientSock);
			if (iterUsr != _connectedUsers.end())
			{
				currUsr = (iterUsr)->second;
			}

			msg = buildReciveMessage(clientSock, msgCode);
			msg->setUser(currUsr);
			addRecievedMessage(msg);

			msgCode = Helper::getMessageTypeCode(clientSock);
		}
		msg = buildReciveMessage(clientSock, Protocol::Request::EXIT_APP);
		msg->setUser(currUsr);
		addRecievedMessage(msg);
	}
	catch(const std::exception& e)
	{
		TRACE("%s", e.what())
		msg = buildReciveMessage(clientSock, Protocol::Request::EXIT_APP);
		msg->setUser(currUsr);
		addRecievedMessage(msg);
	}
}


/*
*	Safley deletes a client from the server (handles it like sign out)
*
*	@param msg the message from the client
*/
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
 *	@param msg the sign in message
 *	@return if the sign in was successful then pointer to the user else null  
*/
User* TriviaServer::handleSignin(RecievedMessage* msg)
{
	std::string username = msg->getValues()[0];
	std::string password = msg->getValues()[1];


	if (_db.isUserAndPassMatch(username , password))
	{
		if (this->getUserByName(username))
		{
			Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_IN) + "2"); //User is already connected
			return nullptr;
		}
		else
		{
			Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_IN) + "0");
			return new User(username, msg->getSock());
		}
	}

		
	Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_IN) + "1"); //Wrong details
	return nullptr;
}


/*
*	This method handles with the sign up request of the user
*
*	@param msg the sign up message
*	@return if the sign up was successful
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


	if (!_db.isUserExists(username))
	{
		_db.addNewUser(username, password, email);

		Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_UP) + "0"); //success
		return true;
	}

	Helper::sendData(msg->getSock(), std::to_string(Protocol::Response::SIGN_UP) + "2"); //username already exists
	return false;
}


/*
*	This method handles with the sign out request of the user (closes everything for him)
*
*	@param msg the sign out message
*/
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


/*
*	This method handles with the user's request of leaving a game
*
*	@param msg the sign out message
*/
void TriviaServer::handleLeaveGame(RecievedMessage* msg)
{
	User* usr = msg->getUser();

	if (usr->leaveGame())
	{
		delete usr->getGame();
	}
}


/*
*	This method handles with the sign out request of the user (closes everything for him)
*
*	@param msg the sign out message
*/
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


/*
*	This method handles with an answer which was sent by the user
*
*	@param msg the message from the client
*/
void TriviaServer::handlePlayerAnswer(RecievedMessage* msg)
{
	User* currUser = msg->getUser();
	Game* game = currUser->getGame();

	int ansNum = std::stoi(msg->getValues()[0]);
	int timeInSeconds = std::stoi(msg->getValues()[1]);

	if (game)
	{
		if (!game->handleAnswerFromUser(currUser, ansNum, timeInSeconds))
		{
			delete game;
		}
	}
}


/*
*	This method handles with a create room request
*
*	@param msg the message from the client
*	@return if the creation was succesful
*/
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


/*
*	This method handles with a close room request
*
*	@param msg the message from the client
*	@return if the sign up was successful
*/
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


/*
*	This method handles with the join room request of the user
*
*	@param msg the join room message
*	@return if the sign up was successful
*/
bool TriviaServer::handleJoinRoom(RecievedMessage* msg)
{
	User* usr = msg->getUser();

	if (usr)
	{
		int roomId = std::stoi(msg->getValues()[0]);
		Room* requestedRoom = this->getRoomById(roomId);

		return usr->joinRoom(requestedRoom);
	}

	return false;
}


/*
*	This method handles with the leave room request of the user
*
*	@param msg the leave room message
*	@return if the operation was successful
*/
bool TriviaServer::handleLeaveRoom(RecievedMessage* msg)
{
	User* usr = msg->getUser();
	
	if (!usr) return false;
	if (!usr->getRoom()) return false;

	usr->leaveRoom();

	return true;
}


/*
*	This method returns all of the users in a room
*
*	@param msg the request message
*/
void TriviaServer::handleGetUserInRoom(RecievedMessage* msg)
{
	Room* r = this->getRoomById(std::stoi(msg->getValues()[0]));
	if(r) Helper::sendData(msg->getSock(), r->getUsersListMessage());
}


/*
*	This method returns all of the rooms to the client
*
*	@param msg the request message
*/
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


/*
*	Sends a top 3 scores and users list to the client
*
*	@param msg the request message
*/
void TriviaServer::handleGetBestScores(RecievedMessage* msg) 
{
	User* usr = msg->getUser();

	if (usr)
	{
		std::vector<std::pair<std::string, int>> scores = _db.getBestScores();
		std::stringstream scoresMsg;

		int count = 0;
		const int numOfUsers = 3;

		scoresMsg << Protocol::Response::BEST_SCORES;


		for (std::pair<std::string, int> i : scores)
		{
			count++;

			scoresMsg << Helper::getPaddedNumber(i.first.length() , 2) << i.first;
			scoresMsg << Helper::getPaddedNumber(i.second , 6);
		}

		for (count; count < numOfUsers; count++) { scoresMsg << Helper::getPaddedNumber(0 , 8); } //in case that there are no 3 users
		

		usr->send(scoresMsg.str());
	}
}


/*
*	This method gets the personal status of a client (total number of questions answered and more statictics)
*
*	@param msg the request message
*/
void TriviaServer::handleGetPersonalStatus(RecievedMessage* msg)
{
	//TODO
}


///////----------------------------- Message Queue --------------------------------///////


/*
*	Takes care of the queue of the RecivedMessages and handles each one of it
*/
void TriviaServer::handleRecievedMessages()
{
	RecievedMessage* msg;
	User* loginUser = nullptr;

	while (true)
	{
		std::unique_lock<std::mutex> lock(_qLock);
		_qCV.wait(lock);

		while (!_qRcvMessages.empty())
		{
			msg = _qRcvMessages.front();
			_qRcvMessages.pop();
			lock.unlock();

			try
			{
				switch (msg->getMessageCode())
				{
				case Protocol::Request::SIGN_IN:
					loginUser = this->handleSignin(msg); //assign to user object

					if (loginUser)
					{
						this->_connectedUsers[msg->getSock()] = loginUser;
						loginUser = nullptr;
					}

					TRACE("Client requested sign in")
						break;

				case Protocol::Request::SIGN_OUT:
					this->handleSignOut(msg);
					TRACE("Client requested sign out")
						break;

				case Protocol::Request::SIGN_UP:
					this->handleSignUp(msg);
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
					this->handleLeaveRoom(msg); //returns bool
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
				case Protocol::Request::EXIT_APP:
					this->safeDeleteUser(msg);
					TRACE("Client requested exit app")
						break;
				default:
					this->safeDeleteUser(msg);
					TRACE("Unknown message code: %d", msg->getMessageCode())
						break;
				}
			}
			catch (std::exception& e)
			{
				this->safeDeleteUser(msg);
			}
			lock.lock();
		}
		lock.unlock();
	}
}


/*
*	Pushes a new RecivedMessage to the queue
*	@param msg the new RecivedMessage to push to the queue
*/
void TriviaServer::addRecievedMessage(RecievedMessage* msg)
{
	std::unique_lock<std::mutex> lock(_qLock);
	_qRcvMessages.push(msg);
	_qCV.notify_all();
}



///////---------------------------------- Getters ----------------------------------///////


/*
*	Gets a Room by id
*
*	@param id the id of the room
*	@return the selected Room obejct's pointer
*/
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


/*
*	Gets a User by its name
*
*	@param name the name of the user
*	@return the selected User obejct's pointer
*/
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


/*
*	Gets a User by its socket
*
*	@param sock the socket of the user
*	@return the selected User obejct's pointer
*/
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