#include "demos.h"
#include <ctime>
using namespace std;

/**TODO: as it turns out, simple scenes arent actually so simple
this is going to be a wip for a bit
need to implement:
-spacial partioning of objects
-lightsources
-file io
-ray tracing
--possibly research metropolis light transport
*/
void demos::simpleScene(){
	MG::engine e;
 	//e.setEventHandler(&checkClose);//moved this to be the default event handler
 	e.initWindow(1280,720);
	e.setTitle("Mongoose Rendering Engine Demo");

	e.setEventAsync(true);

	const long fps=60;

	//load file

	//draw scene at desired framerate
	//long time=time();
	while(1){
		e.update();
	}
}
