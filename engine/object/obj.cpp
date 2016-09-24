#include "object.h"
#include "../core/core.h"
using namespace MG;

obj::obj(engine *e){
	if(e){e->addToScene(this);}
}
