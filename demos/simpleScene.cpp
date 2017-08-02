#include "demos.h"
#include <cmath>
#include <chrono>
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

MG::pointcloud pointbubble(MG::engine *e){
	int npoints=2;
	MG::point pointarr[npoints];

	for(int i=0;i<npoints;i++){
		pointarr[i].col.r=1;
		pointarr[i].col.g=1;
		pointarr[i].col.b=1;

		pointarr[i].pos=MG::vec3d((double(i))/npoints,0,0);
	}

	return MG::pointcloud(e,pointarr,npoints,10);
}

uint64_t ctimeMillis(){
	chrono::microseconds ms=chrono::duration_cast< chrono::milliseconds >(
		chrono::system_clock::now().time_since_epoch()
	);
	return ms.count();
}
double PI=4*atan(1.0);


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
	MG::pointcloud thing=pointbubble(&e);
	printf("cloud made\n");

	e.mainCamera.position.z=-5;

	//draw scene at desired framerate
	while(1){
		if(e.isTimeToUpdate()){
			double t=ctimeMillis()/1000.0;
			//e.mainCamera.position.x=0.5*cos(2*PI*t);
			//e.mainCamera.position.y=0.5*sin(2*PI*t);
			//e.mainCamera.position.z=-2.5*cos(2*PI*t)-3;
			e.update();
		}
	}
}
