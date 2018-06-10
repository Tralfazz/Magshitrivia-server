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

bool User::createRoom(int roomId, std::string roomName, int maxUsers, int questionsNo, int questionTime)
{
	if (!this->_currRoom)
	{
		this->_currRoom = new Room(roomId, this, roomName, maxUsers, questionsNo, questionTime);
		Helper::sendData(this->_sock, std::to_string(Protocol::Response::CREATE_NEW_ROOM) + "0"); //success

		return true;
	}
	else
	{
		Helper::sendData(this->_sock, std::to_string(Protocol::Response::CREATE_NEW_ROOM) + "1"); //fail
		//send faliure message (from here??)

		return false;
	}
}

bool User::joinRoom(Room* newRoom)
{
	if (!this->getRoom() && newRoom)
	{
		if (newRoom->joinRoom(this))
		{
			this->_currRoom = newRoom;
		}
		else
		{
			return false;
		}
	}
	else
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
