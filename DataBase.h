#pragma once

#include <string>
#include <sstream>

#include "sqlite3.h"
#include "Question.h"


class DataBase
{
	private:

		static int callbackCount(void* data, int argc, char** argv, char** cols);
		static int callbackQuestions(void* data, int argc, char** argv, char** cols);
		static int callbackBestScores(void* data, int argc, char** argv, char** cols);
		static int callbackPersonalStatus(void* data, int argc, char** argv, char** cols);

	public:

		DataBase();
		~DataBase();

		bool isUserExists(std::string user);
		bool addNewUser(std::string username, std::string password, std::string email);
		bool isUserAndPassMatch(std::string username, std::string password);

		std::vector<Question*> initQuestions(int questionsNo);
		std::vector<std::string> getBestScores();
		std::vector<std::string> getPersonalStatus(std::string username);

		int insertNewGame();
		bool updateGameStatus(int gameId);
		bool addAnswerToPlayer(int gameId, std::string username, int questionId, std::string answer, bool isCorrect, int answerTime);
};