#include "threadpool.h"
#include <algorithm>
using namespace MG;

void threadPool::statusChecker(){
	for(int i=0;i<runningThreads;i++){
		threads[i]->join();
	}
	if(waiting){finish();}//if it wasnt async, then clean up after self when done running
	isdone=true;
}

int threadPool::getMaxThreads(){
	return std::max(1u,std::thread::hardware_concurrency());
}

void threadPool::finish(){
	//if it was running async, wait for it to finish first before cleaning
	if(statusThread){
		statusThread->join();
		delete statusThread;
		statusThread=0;
	}

	if(threads){
		for(int i=0;i<runningThreads;i++){
			delete threads[i];
			//threads[i]=0;//this probably isnt needed, but who knows it might be in demand sometime
		}
		delete[] threads;
		threads=0;
	}
	isdone=true;
}

threadPool::~threadPool(){finish();}
