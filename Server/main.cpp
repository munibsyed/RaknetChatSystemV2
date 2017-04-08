#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include <algorithm>
#include "ChatServer.h"
#include <vector>

int main()
{
	ChatServer *server = new ChatServer(5456, 32);
	server->Update();

	delete server;

	system("pause");
	
	return 0;
}