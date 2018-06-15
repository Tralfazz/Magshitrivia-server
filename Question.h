#pragma once

#include <string>
#include <ctime>
#include <cstdlib>

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
		Question(int id, std::string q ,std::string correctAns , std::string ans1 , std::string ans2 , std::string ans3);
		
		std::string getQuestion();
		std::string* getAnswers();
		int getCorrectAnswerIndex();
		int getId();

};