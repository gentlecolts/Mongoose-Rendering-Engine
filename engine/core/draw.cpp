/**
this file implements all functionality of the engine class that pertain to rendering the scene
*/
#include "core.h"
using namespace MG;

void engine::update(){
	//check events if they're not running in a separate thread
	if(isEventSync){
		evtProc(&evt);
	}
	//step physics(?)
	//render scene
	//update window
	win.update();
}
