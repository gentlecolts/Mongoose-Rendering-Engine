/**
this file implements all functionality of the engine class that pertain to managing the scene
*/
#include "core.h"
#include "scene.h"
#include <algorithm>
using namespace MG;

sceneContainer* engine::getActiveScene(){
	return &scene;
}

/////FROM sceneContainer CLASS/////

sceneContainer::sceneContainer(){
}

void sceneContainer::render(int numRays,ray *raysIn,ray *raysOut){
	for(int i=0;i<numRays;i++){
		double tmin=INFINITY;
		//std::for_each(objects.begin(),objects.end(),[&raysIn,&raysOut,&i,&tmin](obj* object){
		std::for_each(objects.begin(),objects.end(),[&](obj* object){
			double t;
			ray r_out;
			if(object->bounceRay(raysIn[i],r_out,t) && t<tmin){
				raysOut[i]=r_out;
				tmin=t;
			}
		});
	}
}

void sceneContainer::addObject(obj *o){
	objects.push_back(o);
}

//note that this does not remove the object completely, but instead just flags it for removal
//TODO: make sure update clears objects flagged for removal
void sceneContainer::removeObject(obj *o){
	//*
	toRemove.push(o);
	/*/
	auto iter=std::find(objects.begin(),objects.end(),o);
	if(iter!=objects.end()){
		toRemove.push(iter);
	}//*/
}

void sceneContainer::clearScene(){
}
