#include "RecievedMessage.h"



RecievedMessage::RecievedMessage(SOCKET sc, int msgCode)
{
	this->_sock = sc;
	this->_messageCode = msgCode;
}


RecievedMessage::RecievedMessage(SOCKET sc, int msgCode, std::vector<std::string> values)
	: RecievedMessage(sc , msgCode)
{
	this->_values = values;
}


/*
*	@return Socket: the socket in which the communication is used
*/
SOCKET RecievedMessage::getSock()
{
	return this->_sock;
}


/*
*	@return User: the user which the message is supposed to get to 	
*/
User* RecievedMessage::getUser()
{
	return this->_user;
}


/*
*	Sets a new user according to the input
* 
*	@param user the user object to set the user to
*/
void RecievedMessage::setUser(User* user)
{
	if (user != nullptr)
	{
		this->_user = user;
	}
	else throw std::invalid_argument("Cannot set user to null");
}


/*
*	@return messageCode: the message code of the current message
*/
int RecievedMessage::getMessageCode()
{
	return this->_messageCode;
}


/*
*	@return values: the values of the current message
*/
std::vector<std::string>& RecievedMessage::getValues()
{
	return this->_values;
}