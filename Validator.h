#pragma once

#include <string>


static class Validator
{
	public:
		bool isPasswordValid(std::string& password);
		bool isUsernameValid(std::string& username);
};