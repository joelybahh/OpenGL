#include "CubeApp.h"

int main() {
	
	auto app = new CubeApp();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}