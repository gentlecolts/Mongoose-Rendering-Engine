/**
TODO:
use SDL2 -try to hide it as much as possible, any external libraries should be treated as though they could be dropped any time
use c++11 threading

look into BVH
look into gcc's automated profiling
*/
#include "demos/demos.h"
#include <cstdlib>
#include <iostream>
using namespace std;

void threadtest(){
	while(true){cout<<"thread is running"<<endl;}
}

int main(int argc,char** argv){
	//demos::simpleStartup();
	demos::simpleScene();
	//demos::threadPoolTest();
	return 0;
}
