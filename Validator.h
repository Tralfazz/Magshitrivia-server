#pragma once

#include <string>


static class Validator
{
	public:
		static bool isPasswordValid(std::string& password);
		static bool isUsernameValid(std::string& username);
};