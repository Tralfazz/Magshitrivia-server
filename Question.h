#pragma once

#include <string>


class Question
{
	private:
		int _id;
		int _correctAnswerIndex;

		std::string _question;

		std::string _answer1;
		std::string _answer2;
		std::string _answer3;
		std::string _answer4;
		
	public:
		Question(int id, std::string q ,std::string q1 , std::string q2 , std::string q3 , std::string q4);
		
		std::string getQuestion();
		std::string* getAnswers();
		int getCorrectAnswerIndex();
		int getId();

};