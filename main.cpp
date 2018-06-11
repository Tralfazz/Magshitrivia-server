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
		TRACE("An exception occoured: %s" , e.what());
	}
	
	return 0;
}