#include "$safeprojectname$App.h"

int main() {
	
	auto app = new $safeprojectname$App();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}