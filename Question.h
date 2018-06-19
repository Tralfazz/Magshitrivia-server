#pragma once

#include <string>
#include <ctime>
#include <cstdlib>
#include <deque>
#include <queue>

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

		std::queue<std::string> getRandomQuestionQueue(std::string correctA, std::string a1, std::string a2, std::string a3);
		
	public:
		Question(int id, std::string q ,std::string correctAns , std::string ans1 , std::string ans2 , std::string ans3);
		
		std::string getQuestion();
		std::string* getAnswers();
		int getCorrectAnswerIndex();
		int getId();

};