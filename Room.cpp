#include "Room.h"



Room::Room(int id, User * admin, std::string name, int maxUsers, int questionNo, int questionTime)
	: _id(id), _admin(admin),_name(name), _maxUsers(maxUsers), _questionNo(questionNo),_questionTime(questionTime)
{
	_users.push_back(admin);
}


/*
	Gets all users as a string

	@param usersList the list to iterate over 
	@param excludeUser a user to exclude in the process
*/
std::string Room::getUsersAsString(std::vector<User*> usersList, User* excludeUser)
{
	std::string s;

	s += "\n";
	for (auto user : usersList)
	{
		if (user != excludeUser)
		{
			s += (user->getUsername() + ", ");
		}
	}
	s += "\n";
	return s;
}


/*
Sends a message to all users in the room

@param msg the message to send
*/
void Room::sendMessage(std::string msg)
{
	sendMessage(nullptr, msg);
}


/*
Sends a message to all users in the room excluding the sent user

@param msg the message to send
@param excludeUser
*/
void Room::sendMessage(User* excludeUser, std::string msg)
{
	for (User* u : this->_users)
	{
		if (u != excludeUser)
		{
			Helper::sendData(u->getSocket(), msg);
		}		
	}
}

bool Room::joinRoom(User* user)
{
	if (_users.size() == _maxUsers)
	{
		Helper::sendData(user->getSocket(), std::to_string(Protocol::Response::JOIN_ROOM) + "1"); // failed - room is full
		return false;
	}
	else
	{
		_users.push_back(user);
		Helper::sendData(user->getSocket(), std::to_string(Protocol::Response::JOIN_ROOM) + "0"); // success
		sendMessage(user, getUsersListMesasage());
		return true;
	}
}


void Room::leaveRoom(User* user)
{
}


int Room::closeRoom(User* user)
{
	return 0;
}


/*
	@return all users which are connected to the room
*/
std::vector<User*> Room::getUsers()
{
	return this->_users;
}


std::string Room::getUsersListMesasage()
{
	std::string strUsers;
	std::string tempUname;

	for(auto usr : _users)
	{
		tempUname = usr->getUsername();
		strUsers += tempUname.length();
		strUsers += tempUname;
	}

	return std::to_string(Protocol::Response::USERS_FROM_ROOM + 
		+ this->_users.size()) + strUsers;
}


/*
	@return the number of questions
*/
int Room::getQuestionsNo()
{
	return this->_questionNo;
}


/*
	@return the id of the room
*/
int Room::getId()
{
	return this->_id;
}


/*
	@return the name of the room
*/
std::string Room::getName()
{
	return this->_name;
}


