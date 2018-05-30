#include <exception>
#include "TriviaServer.h"


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
	while (true)
	{
		SOCKET clientSocket = ::accept(this->_socket, NULL, NULL);

		if (clientSocket == INVALID_SOCKET)
			throw std::exception(__FUNCTION__);

		std::thread(&TriviaServer::clientHandler, this, clientSocket).detach();
	}
}



void TriviaServer::server()
{
	this->bindAndListen();

	//while (true)
	//{
		this->accept();
	//}
}


void TriviaServer::clientHandler(SOCKET clientSock)
{
	while(true) Helper::sendData(clientSock, "Hello clinet!");
}




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
