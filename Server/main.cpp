#include <iostream>
#include "ChatServer.h"
#include <iostream>
#include <string>
#include <vector>




int main(int argc, char	**argv)
{
	bool persistentData = false;
	if (argc > 1)
	{
		if (strcmp(argv[1], "-persistent") == 0)
		{
			persistentData = true;
		}
	}

	ChatServer *server = new ChatServer(5456, 32, persistentData);
	server->Update();

	delete server;

	system("pause");
	
	return 0;
}