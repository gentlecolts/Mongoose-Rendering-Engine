#include "object.h"
#include "../core/core.h"
using namespace MG;

//*
obj::obj(const obj &o){
	//file=o.file;//cant copy, would need to re-open
	if(owner=o.owner){owner->addToScene(this);}
}//*/

obj::obj(engine* e,metadata *meta):owner(e){
	if(e){e->addToScene(this);}
}

obj::obj(engine* e,std::string fname,metadata *meta):obj(e,meta){
	//TODO
}

//*
obj::~obj(){
	if(owner){owner->removeFromScene(this);}
}
//*/
