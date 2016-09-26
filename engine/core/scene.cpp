/**
this file implements all functionality of the engine class that pertain to managing the scene
*/
#include "core.h"
using namespace MG;

sceneContainer::sceneContainer(){
}

void engine::addToScene(obj *o){
	scene.objects.push_back(o);
}

//note that this does not remove the object completely, but instead just flags it for removal
//TODO: make sure update clears objects flagged for removal
void engine::removeFromScene(obj *o){
}

void engine::clearScene(){
}
