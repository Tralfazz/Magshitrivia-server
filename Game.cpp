#include "Game.h"


Game::Game(const std::vector<User*>& users, int questionsNo, DataBase& db)
	: _users(users), _questions_no(questionsNo), _db(db)
{
	int gameId = _db.insertNewGame();

	if (gameId == -1)
		throw std::exception("could not insert new game to database (Game c'tor)");
	
	this->_questions = _db.initQuestions(questionsNo);
	//init results

	for (User*& u : _users)
	{
		u->setGame(this);
	}
}


Game::~Game()
{
	_users.clear();

	for (Question* q : _questions)
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

void Game::sendQuestionToAllUsers()
{
	this->sendDataToAllUsers(this->getQuestionMsg(_currQuestionIndex));
}


std::string Game::getQuestionMsg(int qId)
{
	std::stringstream msg;
	std::string* ans;
	std::string que;

	msg << Protocol::Response::QUESTION;

	for (Question* i : _questions)
	{
		if (i->getId() == qId)
		{
			que = i->getQuestion();

			if (que.size() == 0) //send data to admin
			{
				return;
			}

			msg << Helper::getPaddedNumber(que.size(), 3) << que;

			ans = i->getAnswers();

			for (size_t i = 0; i < 4; i++)
			{
				msg << Helper::getPaddedNumber(ans[i].size(), 3) << ans[i];
			}

			delete ans;
		}
	}

	return msg.str();
}


void Game::sendDataToAllUsers(std::string data)
{
	for (User* u : _users)
	{
		try
		{
			u->send(data);
		}
		catch (std::exception& e)
		{
			TRACE("Error in Game::sendQuestionToAllUsers: %s", e.what())
		}
	}
}



void Game::sendFirstQuestion()
{
	this->sendQuestionToAllUsers();
}


void Game::handleFinishGame()
{
}

bool Game::handleNextTurn()
{
	return false;
}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	return false;
}


bool Game::leaveGame(User* currUser)
{
	std::vector<User*>::iterator it = std::find(_users.begin(), _users.end(), currUser);

	if (it != _users.end())
	{
		_users.erase(it);
		this->handleNextTurn();
	}

	//return game is ended
}

int Game::getID()
{
	return 0;
}
