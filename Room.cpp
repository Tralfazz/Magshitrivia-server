#include "Room.h"

Room::Room(int id, User * admin, std::string name, int maxUsers, int questionNo, int questionTime)
	: _id(id), _admin(admin),_name(name), _maxUsers(maxUsers), _questionNo(questionNo),_questionTime(questionTime)
{
	_users.push_back(admin);
}

std::string Room::getUsersAsString(std::vector<User*> usersList, User * excludeUser)
{
	std::string s;

	s += "\n";
	for (auto user : usersList)
	{
		if (user != excludeUser)
		{
			s += (user->getUsername() + ",");
		}
	}
	s += "\n";
	return s;
}

void Room::sendMessage(std::string)
{

}

void Room::sendMessage(User *, std::string)
{
}

bool Room::joinRoom(User *)
{
	return false;
}

void Room::leaveRoom(User *)
{
}

int Room::closeRoom(User *)
{
	return 0;
}

std::vector<User*> Room::getUsers()
{
	return std::vector<User*>();
}

std::string Room::getUsersListMesasage()
{
	return std::string();
}

int Room::getQuestionsNo()
{
	return 0;
}

int Room::getId()
{
	return 0;
}

std::string Room::getName()
{
	return std::string();
}


