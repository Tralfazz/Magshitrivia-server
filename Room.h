#pragma once

#include <vector>
#include <string>

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
	
		std::string getUsersAsString(std::vector<User*>, User*);
		void sendMessage(std::string msg);
		void sendMessage(User* user, std::string msg);


	public:
		Room(int, User*, std::string, int, int, int);
		bool joinRoom(User* user);
		void leaveRoom(User* user);
		int closeRoom(User* user);
		std::vector<User*> getUsers();
		std::string getUsersListMesasage();
		int getQuestionsNo();
		int getId();
		std::string getName();
};