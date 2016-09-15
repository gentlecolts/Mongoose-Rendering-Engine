/**
this file implements all functionality of the event class
*/

#include "event.h"
#include "core.h"
using namespace MG;

event::event(engine *eventParent):parent(eventParent){
	if(eventParent){
		index=eventParent->registerEvent(this);
	}
}
event::~event(){
	parent->removeEvent(index);
}
