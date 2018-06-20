#include "Game.h"


Game::Game(const std::vector<User*>& users, int questionsNo, DataBase& db)
	: _users(users), _questions_no(questionsNo), _db(db)
{
	_id = _db.insertNewGame();

	if (_id == -1)
		throw std::exception("could not insert new game to database (Game c'tor)");
	
	this->_questions = _db.initQuestions(questionsNo);
	//init results

	for (User*& u : _users)
	{
		_results[u->getUsername()] = 0;
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
		if (i == _questions[qId])
		{
			que = i->getQuestion();

			if (que.size() == 0) //send data to admin
			{
				return "";
			}

			msg << Helper::getPaddedNumber(que.size(), 3) << que;

			ans = i->getAnswers();

			for (size_t i = 0; i < 4; i++)
			{
				msg << Helper::getPaddedNumber(ans[i].size(), 3) << ans[i];
			}

			delete[] ans;
		}
	}

	return msg.str();
}


std::string Game::getEndGameMsg()
{
	std::stringstream msg;

	msg << Protocol::Response::END_GAME << Helper::getPaddedNumber(_users.size() , 1);


	for (std::pair<std::string,int> i : _results)
	{
		msg << Helper::getPaddedNumber(i.first.size(), 2) << i.first;
		msg << Helper::getPaddedNumber(i.second, 2);
	}

	return msg.str();
}


std::string Game::getCorrectAnsMsg(bool isCorrect)
{
	std::stringstream msg;
	msg << Protocol::Response::IS_ANSWER_CORRECT << isCorrect ? 1 : 0;

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
	_db.updateGameStatus(_id);

	std::string endGameMsg = this->getEndGameMsg();

	for (User* u : _users)
	{
		try
		{
			u->setGame(nullptr);
			u->send(endGameMsg);
		}
		catch(...){}
	}
}


bool Game::handleNextTurn()
{
	if (_users.empty())
	{
		this->handleFinishGame();

		return true; //game has ended
	}
	else
	{
		if (_currentTurnAnswers == _users.size()) //if all users answered the question
		{
			_currentTurnAnswers = 0;
			_currQuestionIndex++;

			if (_questions_no == _currQuestionIndex) //if the question is the last one
				this->handleFinishGame();
			else
			{
				this->sendQuestionToAllUsers();
			}
		}
	}


	return false; //game is still on
}


bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	bool answeredRight = (answerNo == (_questions[_currQuestionIndex]->getCorrectAnswerIndex() + 1)); //if the user answered the right answer
	std::string ansText = answerNo == 5 ? "" : _questions[_currQuestionIndex]->getAnswers()[answerNo - 1];


	user->send(this->getCorrectAnsMsg(answeredRight)); //if the answer was correct


	if (answeredRight) _results[user->getUsername()]++;
	

	_db.addAnswerToPlayer(_id,
						  user->getUsername(),
						  _currQuestionIndex, 
						  ansText /*if ansNo is 5 then send nothing*/, 
						  answeredRight /*if the question was correct*/,
						  time);



	_currentTurnAnswers++;

	return this->handleNextTurn();
}


bool Game::leaveGame(User* currUser)
{
	std::vector<User*>::iterator it = std::find(_users.begin(), _users.end(), currUser);

	if (it != _users.end())
	{
		_users.erase(it);
	}

	return this->handleNextTurn();;
}


int Game::getID()
{
	return _id;
}