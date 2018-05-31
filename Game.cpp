#include "Game.h"


Game::Game(const std::vector<User*>& users, int questionsNo, DataBase& db)
	: _users(users), _questions_no(questionsNo), _db(db)
{

}


Game::~Game()
{
	this->_users.clear();

	for (Question* q : this->_questions)
	{
		delete q;
	}
}



bool Game::insertGameToDB()
{
	return false;
}

bool Game::initQuestionsFromDB()
{
	return false;
}

void Game::sendQuestionsToAllUsers()
{
}


void Game::sendFirstQuestion()
{
}

void Game::handleFinishGame()
{
}

bool Game::handleNextTurn()
{
	return false;
}

bool Game::handleAnswerFromUser(User*, int, int)
{
	return false;
}

bool Game::leaveGame(User *)
{
	return false;
}

int Game::getID()
{
	return 0;
}
