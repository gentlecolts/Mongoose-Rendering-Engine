/**
this file implements all functionality of the engine class that pertain to handling and reporting events
*/

#include "core.h"
#include "event.h"
using namespace MG;

void loopEvt(void (*evtProc)(event*),event *evt){
    while(1){evtProc(evt);}
}

///////core engine event functions///////

bool engine::setEventAsync(bool b){
	if(b!=isEventSync){//no need to do anything if the state of threaded or not is unchanged
		return true;
	}

	///TODO: learn how c++11 threading works
	if(b){//create event thread
		evtThread=new std::thread(loopEvt,this.pollEvents,&evt);
		evtThread->detach();
	}else{//terminate event thread if it exists
		delete evtThread;
	}

	//if we've reached this point, then everything went well and we can return
	isEventSync=!b;
	return true;//everything went well
}

void engine::pollEvents(){
	//TODO:iterate through all events, ideally reduce number of checks that need to be done to see if events are called
}

///////event class functions///////

