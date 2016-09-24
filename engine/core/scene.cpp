/**
this file implements all functionality of the engine class that pertain to managing the scene
*/
#include "core.h"
using namespace MG;

void engine::addToScene(obj *o){
	objects.push_back(o);
}

void engine::updateTasks(int id,int numThreads){
}
