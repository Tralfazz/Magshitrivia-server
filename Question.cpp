#include "Question.h"



/*

*/
Question::Question(int id, std::string q, std::string correctAns, std::string ans1, std::string ans2, std::string ans3)
	: _id(id) , _question(q)
{
	//TODO randomize which answers gets into where and get the correct ans index 
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

