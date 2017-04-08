#include "Client.h"

int main() {
	
	auto app = new Client();
	app->run("AIE", 500, 440, false);
	delete app;

	return 0;
}