#include "Validator.h"


/*
	This method checks if a given password is valid 


	 valid password must be with at least 4 characters, 
	 must not contain spaces and must contain at least one digit,
	 one capital letter and one lower case letter


	 @param password the string to check its validtility

	 @return if the string is valid for a password
*/
bool Validator::isPasswordValid(std::string& password)
{
	std::regex passwordPattern(R"(^(?=.[A-Z])(?=.[a-z])(?=.*[1-9])\S+$)");
	return std::regex_match(password, passwordPattern);
}



/*
	This method checks if a given username is valid
	
	valid username must start with a character, cannot contain spaces 
	and cannot be empty
	
	
	@param password the string to check its validtility
	@return if the string is valid for a password
*/
bool Validator::isUsernameValid(std::string& username)
{
	std::regex usernamePattern(R"(^[a-zA-Z][\w]+$)");
	return std::regex_match(username, usernamePattern);
}
