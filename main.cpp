#include <iostream>
#include <exception>

#include "TriviaServer.h"
#include "WSAInitializer.h"


int main()
{
	try
	{
		WSAInitializer ws;
		TriviaServer s;
		s.server();
	}
	catch (std::exception& e)
	{
		std::cout << "An exception occoured: " << e.what() << std::endl;
	}
	
	return 0;
}