#include "Question.h"



/*

*/
Question::Question(int id, std::string q ,std::string a1, std::string a2, std::string a3, std::string a4)
	: _id(id) , _question(q) , _answer1(a1) , _answer2(a2) , _answer3(a3) , _answer4(a4)
{

}



/*

*/
std::string Question::getQuestion() 
{
	return this->_question;
}



/*
*/
std::string* Question::getAnswers()
{
	return nullptr;
}



/*

*/
int Question::getCorrectAnswerIndex()
{
	return this->_correctAnswerIndex;
}


/*

*/
int Question::getId()
{
	return this->_id;
}

