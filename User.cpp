#include "User.h"

User::User(std::string username, SOCKET sock)
	: _username(username), _sock(sock), _currGame(nullptr), _currRoom(nullptr)
{
}

void User::send(std::string message)
{
	Helper::sendData(this->_sock, message);
}

std::string User::getUsername()
{
	return this->_username;
}

SOCKET User::getSocket()
{
	return this->_sock;
}

Room* User::getRoom()
{
	return this->_currRoom;
}

Game* User::getGame()
{
	return this->_currGame;
}

void User::setGame(Game* gm)
{
	this->_currGame = gm;
	this->_currRoom = nullptr;
}

void User::clearGame()
{
	this->_currGame = nullptr;
}	

bool User::createRoom(int, std::string, int, int, int)
{
	return false;

	this->closeRoom();
	//this->_currRoom = new Room(int , this )

	//if (_currRoom)
	//	// msg code 114
	//else
	//	Protocol::Response::;//msg code 114
}

bool User::joinRoom(Room* newRoom)
{
	if (this->_currGame != nullptr)
	{
		this->_currRoom = newRoom;
		return true;
	}


	return false;
}

void User::leaveRoom()
{

}

int User::closeRoom()
{
	return 0;
}

bool User::leaveGame()
{
	return false;
}
