#pragma once


namespace Protocol 
{
	const int PORT = 8080;

	class Request
	{
		public:

			static const int SIGN_IN = 200;
			static const int SIGN_OUT = 201;
			static const int SIGN_UP = 203;

			static const int EXISTING_ROOMS = 205;
			static const int USERS_FROM_ROOM = 207;

			static const int JOIN_ROOM = 209;
			static const int LEAVE_ROOM = 211;

			static const int CREATE_NEW_ROOM = 213;
			static const int CLOSE_ROOM = 215;

			static const int BEGIN_GAME = 217;

			static const int GAME_CLIENT_ANSWER = 219;

			static const int LEAVE_GAME = 222;
			static const int BEST_SCORES = 223;

			static const int PERSONAL_MODE = 225;
			
			static const int EXIT_APP = 299;
	};	

	
	//TODO add consts for success and failure 
	class Response 
	{
		public:

			static const int SIGN_IN = 102;
			static const int SIGN_UP = 104;
			
			static const int EXISTING_ROOMS = 106;
			static const int USERS_FROM_ROOM = 108;
			 
			static const int JOIN_ROOM = 110;
			static const int LEAVE_ROOM = 112;
			
			static const int CREATE_NEW_ROOM = 114;
			static const int CLOSE_ROOM = 116;
			 
			static const int QUESTION = 118;
			static const int IS_ANSWER_CORRECT = 120;
			static const int END_GAME = 121;
			 
			static const int BEST_SCORES = 124;
			
			static const int PERSONAL_MODE = 126;
	};
}