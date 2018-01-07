#include "object.h"
#include "../core/core.h"
using namespace MG;

//*
obj::obj(const obj &o){
	//file=o.file;//cant copy, would need to re-open

	//copy o's owner, then if it's not null, add self to o's scene
	if(owner=o.owner){o.owner->addObject(this);}
}//*/

obj::obj(sceneContainer* sc,metadata *meta):owner(sc){
	if(sc){sc->addObject(this);}
}

obj::obj(sceneContainer* sc,std::string fname,metadata *meta):obj(sc,meta){
	//TODO
}

//*
obj::~obj(){
	if(owner){owner->removeObject(this);}
}
//*/
