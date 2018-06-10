#pragma once

#include <string>
#include <regex>


class Validator
{
	public:
		static bool isPasswordValid(std::string& password);
		static bool isUsernameValid(std::string& username);
};