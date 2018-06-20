#include "Question.h"



std::queue<std::string> Question::getRandomQuestionQueue(std::string correctA, std::string a1, std::string a2, std::string a3)
{
	int currRand;
	std::queue<std::string> res;
	std::deque<std::string> temp = { correctA,a1,a2,a3 };
	int initialSize = temp.size();

	srand(time(NULL));

	for (int i = 0; i < initialSize; i++)
	{
		currRand = rand() % temp.size();
		if (temp[currRand] == correctA)
		{
			_correctAnswerIndex = i;
		}
		res.push(temp[currRand]);
		temp.erase(temp.begin() + currRand);
	}

	return res;
}

/*

*/
Question::Question(int id, std::string q, std::string correctAns, std::string ans1, std::string ans2, std::string ans3)
	: _id(id) , _question(q)
{
	std::queue<std::string> randAns = getRandomQuestionQueue(correctAns, ans1, ans2, ans3);

	_answer1 = randAns.front();
	randAns.pop();
	_answer2 = randAns.front();
	randAns.pop();
	_answer3 = randAns.front();
	randAns.pop();
	_answer4 = randAns.front();
	randAns.pop();
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

