/**
this file implements all functionality of the engine class that pertain to handling and reporting events
*/

#include "core.h"
using namespace MG;

void engine::setEventHandler(void (*eventProcessor)(SDL_Event*)){
	evtProc=eventProcessor;
}
void engine::setEventHandler(void (*eventProcessor)(SDL_Event*),bool b){
	evtProc=eventProcessor;
	setEventAsync(b);
}

bool engine::setEventAsync(bool b){
	if(b!=isEventSync){//no need to do anything if the state of threaded or not is unchanged
		return true;
	}

	///TODO: learn how c++11 threading works
	if(b){//create event thread
		evtThread=new std::thread(*evtProc);
	}else{//terminate event thread if it exists
		delete evtThread;
	}

	//if we've reached this point, then everything went well and we can return
	isEventSync=!b;
	return true;//everything went well
}
