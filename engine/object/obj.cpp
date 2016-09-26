#include "object.h"
#include "../core/core.h"
using namespace MG;

obj::obj(engine *e,metadata *meta){
	if(e){e->addToScene(this);}
}
