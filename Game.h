#pragma once

#include <vector>
#include <map>
#include "User.h"
#include "Question.h"
#include "DataBase.h"


class User;

class Game
{
	private:

		std::vector<Question*> _questions;
		std::vector<User*> _users;
		int _questions_no;
		int _currQuestionIndex;
		DataBase& _db;
		std::map<std::string, int> _results;
		int currentTurnAnswers;


		bool insertGameToDB();
		bool initQuestionsFromDB();
		void sendQuestionToAllUsers();

		std::string getQuestionMsg(int qId);
		void sendDataToAllUsers(std::string data);


	public:

		Game(const std::vector<User*>& users , int questionsNo, DataBase& db);
		~Game();
		void sendFirstQuestion();
		void handleFinishGame();
		bool handleNextTurn();
		bool handleAnswerFromUser(User* user, int answerNo, int time);
		bool leaveGame(User* currUser);
		int getID();
};