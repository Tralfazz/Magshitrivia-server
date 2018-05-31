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
		void sendMessage(std::string);
		void sendMessage(User*, std::string);


	public:
		Room(int, User*, std::string, int, int, int);
		bool joinRoom(User*);
		void leaveRoom(User*);
		int closeRoom(User*);
		std::vector<User*> getUsers();
		std::string getUsersListMesasage();
		int getQuestionsNo();
		int getId();
		std::string getName();
};