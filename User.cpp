#include "User.h"

User::User(std::string username, SOCKET sock)
	: _username(username), _sock(sock), _currGame(nullptr), _currRoom(nullptr)
{
}

void User::send(std::string message)
{
	Helper::sendData(_sock, message);
}

std::string User::getUsername()
{
	return _username;
}

SOCKET User::getSocket()
{
	return _sock;
}

Room * User::getRoom()
{
	return _currRoom;
}

Game * User::getGame()
{
	return _currGame;
}

void User::setGame(Game * gm)
{
	_currGame = gm;
	_currRoom = nullptr;
}

void User::clearGame()
{
	_currGame = nullptr;
}

bool User::createRoom(int, std::string, int, int, int)
{
	if (_currRoom)
		;// msg code 114
	else
		Protocol::Response::;//msg code 114
}

bool User::joinRoom(Room * newRoom)
{
	return ;
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
