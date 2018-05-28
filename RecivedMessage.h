#pragma once

#include <WinSock2.h>
#include <vector>
#include <string>
#include "User.h"

class RecivedMessage
{
	private:
		SOCKET _sock;
		User* _user;
		int _messageCode;
		std::vector<std::string> _values;

	public:

		SOCKET getSock();
		User* getUser();
		void setUser(User* user);
		int	getMessageCode();
		std::vector<std::string>& getValues();
};