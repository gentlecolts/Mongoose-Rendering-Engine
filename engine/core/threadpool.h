#ifndef THREADPOOL_H_INCLUDED
#define THREADPOOL_H_INCLUDED

#include <thread>
#include <cstdarg>

namespace MG{
	class threadPool{
		std::thread **threads=0;
		std::thread *statusThread=0;
		int runningThreads=0;
		bool waiting;

		void statusChecker(){
			for(int i=0;i<runningThreads;i++){
				threads[i]->join();
			}
			if(waiting){finish();}//if it wasnt async, then clean up after self when done running
			isdone=true;
		}
	public:
		bool isdone=true;

		template<typename... _Args>
		explicit threadPool(void (*pfunc)(int,int,_Args...),int threadcount,bool waitUntilDone,_Args... params):waiting(waitUntilDone){
			//if the value passed in for threadcount is a valid value, use it, otherwise default to the number of avaliable system threads
			runningThreads=(threadcount>0)?threadcount:std::thread::hardware_concurrency();
			threads=new std::thread*[runningThreads];
			isdone=false;

			//start all of the threads
			for(int i=0;i<runningThreads;i++){
				threads[i]=new std::thread(pfunc,i,runningThreads,params...);
			}

			if(waitUntilDone){
				statusChecker();
			}else{
				statusThread=new std::thread(&threadPool::statusChecker,this);
			}
		}
		template<typename T,typename... _Args>
		explicit threadPool(void (T::*pfunc)(int,int,_Args...),int threadcount,bool waitUntilDone,T* classptr,_Args... params):waiting(waitUntilDone){
			//if the value passed in for threadcount is a valid value, use it, otherwise default to the number of avaliable system threads
			runningThreads=(threadcount>0)?threadcount:std::thread::hardware_concurrency();
			runningThreads=runningThreads?runningThreads:1;//hardware_concurrency CAN return 0 in some cases, in which case just run a single thread
			threads=new std::thread*[runningThreads];
			isdone=false;

			//start all of the threads
			for(int i=0;i<runningThreads;i++){
				threads[i]=new std::thread(pfunc,classptr,i,runningThreads,params...);
			}

			if(waitUntilDone){
				statusChecker();
			}else{
				statusThread=new std::thread(&threadPool::statusChecker,this);
			}
		}

		void finish(){
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

		~threadPool(){
			finish();
		}
	};
}

#endif // THREADPOOL_H_INCLUDED
