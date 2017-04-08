#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include <algorithm>
#include "ChatServer.h"
#include <vector>
//FIGURE OUT FILE TRANSFER

//dependencies/raknet/include
/*
1. Extend the program from the tutorial to work like a simple chat server.

a. Add a new message type for chat messages.

b. The client should send a chat message packet when requested by the user.

c. When the server receives a chat message, it should forward the message on to all connected users.
*/

int main()
{
	ChatServer *server = new ChatServer(5456, 32);
	server->Update();

	delete server;

	/*Server *server = new Server(5456, 32);
	server->Update();

	delete server;*/

	system("pause");
	
	return 0;
}