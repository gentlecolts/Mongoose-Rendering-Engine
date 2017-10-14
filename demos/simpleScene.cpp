#include "demos.h"
#include <cmath>
#include <random>
#include <chrono>
#include <cmath>
#include <fstream>
#include <map>
#include <algorithm>
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

random_device generator;
uniform_real_distribution<double> randnum(-1.0,1.0);
uniform_real_distribution<double> randcol(0,1.0);

double next(uniform_real_distribution<double> &rng){
	return rng(generator);
}

MG::pointcloud pointbubble(MG::engine *e,int npoints=1000,int sizeAssert=0){
	MG::point pointarr[npoints];

	for(int i=0;i<npoints;i++){
		pointarr[i].col.r=next(randcol);
		pointarr[i].col.g=next(randcol);
		pointarr[i].col.b=next(randcol);

		pointarr[i].scale=0.025*MG::vec3d(1,1,1);

		//const double x=2*(double(i))/npoints-1;
		//const double x=(double(i))/npoints;
		//pointarr[i].pos=MG::vec3d(x,0,0);
		pointarr[i].pos=MG::vec3d(next(randnum),next(randnum),next(randnum));
	}

	return MG::pointcloud(e,pointarr,npoints,1,sizeAssert);
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
	e.mainCamera.position.z=-3;

	#define BENCHMARK 1

	#if BENCHMARK
	ofstream of("bench.csv");

	for(int i=10;i<10000;i+=i/5){
		map<int,long> tests;
		for(int j=2;j<pow(i,1.0/3);j+=2){
			//printf("testing %i elements and %i cells\n",i,j);
			MG::pointcloud thing=pointbubble(&e,i,j);

			long t=clock();
			for(int k=0;k<10;k++){
				e.update();
			}
			t=clock()-t;
			tests[j]=t;
		}

		auto mintime=min_element(tests.begin(),tests.end(),[&tests](const pair<int,long> &test,const pair<int, long> &old){
			return test.second<old.second;
		});

		of<<i<<','<<mintime->first<<','<<mintime->second<<endl;
	}

	#else
	//generate some objects
	MG::pointcloud thing=pointbubble(&e);
	printf("cloud made\n");

	int counter=0,framecount=100;

	//draw scene at desired framerate
	while(1){
		if(e.isTimeToUpdate()){
			double t=ctimeMillis()/1000.0;
			e.mainCamera.position.x=0.5*cos(2*PI*t/3000);
			e.mainCamera.position.y=0.5*sin(2*PI*t/3000);
			//e.mainCamera.position.z=-2.5*cos(2*PI*t/6000)-3;
			e.update();

			++counter;
			if(counter==framecount){
				break;
			}
		}
	}
	#endif
}
