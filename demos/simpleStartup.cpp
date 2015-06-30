#include "demos.h"
using namespace std;

void demos::simpleStartup(){
	MG::engine e;
 	//e.setEventHandler(&checkClose);//moved this to be the default event handler
 	e.initWindow(1280,720);
	e.setTitle("Mongoose Rendering Engine Demo");

	e.setEventAsync(true);
	//e.setEventAsync(false);
	/*
	thread t(threadtest);
	t.detach();
	//*/

	while(1){
		e.update();
	}
}
