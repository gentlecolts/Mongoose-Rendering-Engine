#include "demos.h"
#include <cmath>
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

class cube:MG::obj{
double rotVel;
public:
	cube(MG::engine* e,double rotationalVel):MG::obj(e),rotVel(rotationalVel){}
};

MG::pointcloud pointbubble(){
	int npoints=100;
	MG::point pointarr[npoints];

	for(int i=0;i<npoints;i++){
	}
}

void demos::simpleScene(){
	MG::engine e;
 	//e.setEventHandler(&checkClose);//moved this to be the default event handler
 	//e.initWindow(1280,720);
 	e.initWindow(640,480);
 	//e.initWindow(641,481);
	e.setTitle("Mongoose Rendering Engine Demo");

	e.setEventAsync(true);

	e.targetFPS=60;

	//generate some objects

	//draw scene at desired framerate
	while(1){
		if(e.isTimeToUpdate()){
			e.update();
		}
	}
}
