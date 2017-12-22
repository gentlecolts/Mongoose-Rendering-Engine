#include "demos.h"
#include <cmath>
#include <random>
#include <chrono>
#include <cmath>
#include <fstream>
#include <map>
#include <algorithm>
#include <sstream>
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

MG::pointcloud pointbubble(MG::engine *e,const int npoints=1000,const int sizeAssert=0){
	MG::point* pointarr=new MG::point[npoints];

	for(int i=0;i<npoints;i++){
		pointarr[i].col.r=next(randcol);
		pointarr[i].col.g=next(randcol);
		pointarr[i].col.b=next(randcol);

		pointarr[i].scale=0.01*MG::vec3d(1,1,1);

		//const double x=2*(double(i))/npoints-1;
		//const double x=(double(i))/npoints;
		//pointarr[i].pos=MG::vec3d(x,0,0);
		pointarr[i].pos=MG::vec3d(next(randnum),next(randnum),next(randnum));
	}

	MG::pointcloud p(e,pointarr,npoints,1,sizeAssert);
	delete[] pointarr;
	return p;
}


MG::pointcloud pointgyro(MG::engine *e,const int npoints=1000,const int sizeAssert=0){
	int n=npoints/6;
	MG::point* pointarr=new MG::point[6*n];

	const double pi=4*atan(1.0);

	MG::color cols[6]={
		MG::color(1,0,0),
		MG::color(.5,0,0),
		MG::color(0,1,0),
		MG::color(0,.5,0),
		MG::color(0,0,1),
		MG::color(0,0,.5)
	};

	const double r=.5;

	for(int i=0;i<n;i++){
		const double
			theta=pi/n,
			c=r*cos(theta),
			s=r*sin(theta);
		for(int j=0;j<6;j++){
			pointarr[6*i+j].scale=.1*MG::vec3d(1,1,1);
			pointarr[6*i+j].col=cols[j];
		}

		pointarr[6*i].pos=MG::vec3d(0,s,c);
		pointarr[6*i+1].pos=MG::vec3d(0,-s,c);
		pointarr[6*i+2].pos=MG::vec3d(c,0,s);
		pointarr[6*i+3].pos=MG::vec3d(c,0,-s);
		pointarr[6*i+4].pos=MG::vec3d(c,s,0);
		pointarr[6*i+5].pos=MG::vec3d(c,-s,0);
	}

	MG::pointcloud p(e,pointarr,6*n,1,sizeAssert);
	delete[] pointarr;
	return p;
}

uint64_t ctimeMillis(){
	chrono::milliseconds ms=chrono::duration_cast< chrono::milliseconds >(
		chrono::system_clock::now().time_since_epoch()
	);
	return ms.count();
}
double PI=4*atan(1.0);


void demos::simpleScene(){
	#define BENCHMARK 0

	MG::engine e;
 	//e.setEventHandler(&checkClose);//moved this to be the default event handler
 	//e.initWindow(1280,720);
 	#if BENCHMARK
 	e.initWindow(320,240);
	e.mainCamera.position.z=-2.2;
 	#else
 	e.initWindow(640,480);
	e.mainCamera.position.z=-3;
 	#endif
 	//e.initWindow(641,481);
	e.setTitle("Mongoose Rendering Engine Demo");

	e.setEventAsync(true);
	e.targetFPS=60;

	#if BENCHMARK
	ofstream of("bench.csv");

	for(int i=100;i<20000;i+=i/8){
		map<int,long> tests;

		//eliminate some of the slowest cases
		const int jlow=(i>2000)?log(i/2.)/1.5:max((int)sqrt(sqrt(3.)*i/60.),2);

		for(int j=jlow;j<pow(i,1.0/3);j++){
			//printf("testing %i elements and %i cells\n",i,j);
			MG::pointcloud thing=pointbubble(&e,i,j);

			stringstream title;
			title<<"Mongoose Bench, ("<<i<<", "<<j<<")";

			e.setTitle(title.str().c_str());

			long t=clock();
			for(int k=0;k<15;k++){
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
	MG::pointcloud thing=pointbubble(&e,10000);
	//MG::pointcloud thing=pointgyro(&e,100);
	printf("cloud made\n");

	int counter=0,framecount=100;

	//draw scene at desired framerate
	while(1){
		if(e.isTimeToUpdate()){
			double t=ctimeMillis()/1000.0;

			#define CAMTYPE 1

			#if CAMTYPE==0
			e.mainCamera.position.x=0.5*cos(2*PI*t/3);
			e.mainCamera.position.y=0.5*sin(2*PI*t/3);
			//e.mainCamera.position.z=-2.5*cos(2*PI*t/6000)-3;
			#elif CAMTYPE==1
			t/=5;
			double r=3;
			e.mainCamera.position.x=cos(2*PI*t/3);
			e.mainCamera.position.z=sin(2*PI*t/3);
			e.mainCamera.position.y=0.5*sin(2*PI*t);
			e.mainCamera.position=e.mainCamera.position.getNormalized()*r;
			#elif CAMTYPE==2
			double r=2,
				x=-r,
				y=r,
				z=-r;
			MG::vec3d pos(x,y,z);

			MG::vec3d off(next(randnum),next(randnum),next(randnum));
			off*=0.01;

			e.mainCamera.position=pos+off;
			#endif

			e.mainCamera.lookAt(MG::vec3d(0,0,0),MG::vec3d(0,1,0));

			e.update();

			#if 1
			stringstream s;
			s<<"Mongoose Rendering Engine Demo ("
			<<e.mainCamera.position.x<<", "
			<<e.mainCamera.position.y<<", "
			<<e.mainCamera.position.z<<")";

			e.setTitle(s.str().c_str());
			#endif

			++counter;
			if(counter==framecount){break;}
		}
	}
	#endif
}
