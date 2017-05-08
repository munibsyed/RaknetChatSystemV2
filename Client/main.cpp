#include "Client.h"

int main() {
	
	auto app = new Client();
	app->run("Munib's Chat System", 500, 440, false);
	delete app;

	return 0;
}