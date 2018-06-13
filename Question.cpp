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
	return _question;
}



/*
*/
std::string* Question::getAnswers()
{
	return new std::string[4]{ _answer1 , _answer2 , _answer3 , _answer4 };
}



/*

*/
int Question::getCorrectAnswerIndex()
{
	return _correctAnswerIndex;
}


/*

*/
int Question::getId()
{
	return _id;
}

