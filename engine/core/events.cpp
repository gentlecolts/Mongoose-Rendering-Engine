/**
this file implements all functionality of the engine class that pertain to handling and reporting events
*/

#include "core.h"
using namespace MG;

void engine::setEventHandler(void (*eventProcessor)(SDL_Event*)){
	evtProc=eventProcessor;
}
