#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "User.h"

class User;

class Room
{
	private:
		std::vector<User*> _users;
		User* _admin;
		int _maxUsers;
		int _questionTime;
		int _questionNo;
		std::string _name;
		int _id;
	
		std::string getUsersAsString(std::vector<User*> usersList, User* excludeUser);
		void sendMessage(std::string msg);
		void sendMessage(User* user, std::string msg);


	public:

		Room(int id, User * admin, std::string name, int maxUsers, int questionNo, int questionTime);

		bool joinRoom(User* user);
		void leaveRoom(User* user);
		int closeRoom(User* user);

		std::vector<User*> getUsers();
		std::string getUsersListMessage();

		int getQuestionsTime();
		int getQuestionsNo();
		int getId();
		std::string getName();

		bool isAdmin(User* user);
};