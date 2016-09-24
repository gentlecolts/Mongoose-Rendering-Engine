/**
this file implements all functionality of the engine class that pertain to core functionality, such as initialization
*/

#include "core.h"
using namespace MG;

engine::engine():event(0){
	init();
	//index and parent for event superclass
	index=registerEvent(this);//this is probably just 0, but you never know
	parent=this;
}
engine::~engine(){
}

void engine::init(){
}
