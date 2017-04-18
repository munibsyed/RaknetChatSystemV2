#include <iostream>
#include "ChatServer.h"

int main()
{
	ChatServer *server = new ChatServer(5456, 32);
	server->Update();

	delete server;

	system("pause");
	
	return 0;
}