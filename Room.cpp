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
	try
	{
		for (User* u : this->_users)
		{
			if (u != excludeUser)
			{
				Helper::sendData(u->getSocket(), msg);
			}
		}
	}
	catch (const std::exception& e)
	{
		TRACE("%s" , e.what())
	}
}

bool Room::joinRoom(User* user)
{
	std::stringstream msg;
	msg << Protocol::Response::JOIN_ROOM;

	if (_users.size() == _maxUsers)
	{
		msg << 1; // failure message

		Helper::sendData(user->getSocket(), msg.str()); // failed - room is full
		return false;
	}
	else
	{
		_users.push_back(user);
		msg << 0 << Helper::getPaddedNumber(_questionNo, 2) << Helper::getPaddedNumber(_questionTime, 2); // success

		Helper::sendData(user->getSocket(), msg.str());
		sendMessage(user, getUsersListMessage());
		return true;
	}
}


void Room::leaveRoom(User* user)
{
	int count = 0;
	for (auto usr : _users)
	{
		if (usr == user)
		{
			_users.erase(_users.begin() + count);
			Helper::sendData(user->getSocket(), std::to_string(Protocol::Response::LEAVE_ROOM) + "0");
			sendMessage(user, getUsersListMessage());
		}
		count++;
	}
}


int Room::closeRoom(User* user)
{
	if (user == _admin)
	{
		sendMessage(std::to_string(Protocol::Response::CLOSE_ROOM));
		for (auto usr : _users)
		{
			if (usr != _admin)
			{
				usr->clearRoom();
			}
		}
		return _id;
	}
	else
		return -1;
}


/*
	@return all users which are connected to the room
*/
std::vector<User*> Room::getUsers()
{
	return this->_users;
}



std::string Room::getUsersListMessage()
{
	std::stringstream users;
	users << Protocol::Response::USERS_FROM_ROOM;


	if (!_admin->getRoom()) //or game has started
	{
		users << 0;
	}
	else
	{
		users << Helper::getPaddedNumber(this->getUsers().size(), 1);

		for (User* u : this->getUsers())
		{
			users << Helper::getPaddedNumber(u->getUsername().length(), 2) << u->getUsername();
		}
	}

	return users.str();
}

int Room::getQuestionsTime()
{
	return this->_questionTime;
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


