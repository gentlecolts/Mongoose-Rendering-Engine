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

		void statusChecker();
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

		static int getMaxThreads();

		void finish();
		~threadPool();
	};
}

#endif // THREADPOOL_H_INCLUDED
