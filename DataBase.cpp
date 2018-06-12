#include "DataBase.h"


const std::string DataBase::DBNAME = "Quiz.db";


DataBase::DataBase()
{
	int rc;
	std::stringstream errMsg;

	rc = sqlite3_open(DataBase::DBNAME.c_str(), &this->_connection);

	if (rc)
	{
		errMsg << "Could not open database: " << sqlite3_errmsg(this->_connection);
		sqlite3_close(this->_connection);


		throw std::exception(errMsg.str().c_str());
	}
}



DataBase::~DataBase()
{
	sqlite3_close(this->_connection);
}



bool DataBase::isUserExists(std::string user)
{
	std::stringstream query;
	int rowsAffected = -1;

	query << "SELECT * FROM t_users WHERE username = " << user << ";";

	if (sqlite3_exec(this->_connection, query.str().c_str(), callbackCount, &rowsAffected, NULL))
	{
		std::stringstream errMsg;
		errMsg << "Could not confirm user from database: " << sqlite3_errmsg(this->_connection);

		throw std::exception(errMsg.str().c_str());
	}

	return rowsAffected != 0;
}


bool DataBase::addNewUser(std::string username, std::string password, std::string email)
{
	std::stringstream query;
	int rowsAffected = -1;

	query << "INSERT INTO t_users(username , password , email) VALUES('" << username << "','" << password << "','" << email << "');";

	return !sqlite3_exec(this->_connection, query.str().c_str(), NULL, &rowsAffected, NULL) ? true : false;
}


bool DataBase::isUserAndPassMatch(std::string username, std::string password)
{
	std::stringstream query;
	int rowsAffected = -1;

	query << "SELECT * FROM t_users WHERE username = '" << username << "' AND password = " << password << ";";

	if (sqlite3_exec(this->_connection, query.str().c_str(), callbackCount, &rowsAffected, NULL))
	{
		std::stringstream errMsg;
		errMsg << "Could not find if username and password match: " << sqlite3_errmsg(this->_connection);

		throw std::exception(errMsg.str().c_str());
	}


	return rowsAffected != 0;
}



std::vector<Question*> DataBase::initQuestions(int questionsNo)
{
	return std::vector<Question*>();
}



std::vector<std::string> DataBase::getBestScores()
{
	return std::vector<std::string>();
}



std::vector<std::string> DataBase::getPersonalStatus(std::string username)
{
	return std::vector<std::string>();
}



int DataBase::insertNewGame()
{
	const std::string query = "INSERT INTO t_games(status,start_time,end_time) values (0 , NOW , NULL);";


	if (sqlite3_exec(this->_connection, query.c_str(), NULL, NULL, NULL))
	{
		std::stringstream errMsg;
		errMsg << "Could not insert new game to database: " << sqlite3_errmsg(this->_connection);

		throw std::exception(errMsg.str().c_str());
	}

	
	return sqlite3_last_insert_rowid(this->_connection);
}



bool DataBase::updateGameStatus(int gameId)
{
	std::stringstream query;
	query << "UPDATE TABLE t_games SET status = 1 , end_time = NOW WHERE game_id = " << gameId << ";";

	return sqlite3_exec(this->_connection , query.str().c_str() , NULL , NULL , NULL);
}



bool DataBase::addAnswerToPlayer(int gameId, std::string username, int questionId, std::string answer, bool isCorrect, int answerTime)
{
	return false;
}



///////---------------------------------- Callbacks ----------------------------------///////


int DataBase::callbackCount(void* data, int argc, char** argv, char** cols)
{
	*((int*)data) = argc;
	return 0;
}


int DataBase::callbackQuestions(void* data, int argc, char** argv, char** cols)
{
	return 0;
}


int DataBase::callbackBestScores(void* data, int argc, char** argv, char** cols)
{
	return 0;
}


int DataBase::callbackPersonalStatus(void* data, int argc, char** argv, char** cols)
{
	return 0;
}