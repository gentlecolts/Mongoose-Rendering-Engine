#ifndef THREADPOOL_H_INCLUDED
#define THREADPOOL_H_INCLUDED

#include <thread>

namespace MG{
	class empty{};
	template<typename T=empty>
	class threadPool{
		int systemThreads;
		void (*fn)(int,int,void*);//index, total threads, params
		void (T::*classfn)(int,int,void*);//class ptr, index, total threads, params
		bool isclass;
		T* classptr;

		class pooledThread{
		public:
			std::thread* mythread=0;
			bool isrunning=false,isdone=true;

			//note that execution begins at this constructor
			pooledThread(int index,int totalThreads,void* params,void (*pfunc)(int,int,void*)){
				mythread=new std::thread(pfunc,index,totalThreads,params);
				mythread->detach();
			}
			pooledThread(T* ptr,int index,int totalThreads,void* params,void (T::*pfunc)(int,int,void*)){
				mythread=new std::thread(pfunc,ptr,index,totalThreads,params);
				mythread->detach();
			}

			void stop(){
				delete mythread;
			}
			~pooledThread(){
				stop();
			}
		};

		pooledThread** threads=0;
		int runningThreads=0;

		void statusChecker(){
			for(int i=0;i<runningThreads;i++){
				threads[i]->mythread->join();
			}
			isdone=true;
		}
		std::thread* statusThread=0;
	public:
		int threadsInPool;
		bool isdone;
		threadPool(void (*pfunc)(int,int,void*),int threadcount=-1):isclass(false),fn(pfunc),threadsInPool(threadcount){
			systemThreads=std::thread::hardware_concurrency();
		}
		threadPool(T* instance,void (T::*pfunc)(int,int,void*),int threadcount=-1):isclass(true),classptr(instance),classfn(pfunc),threadsInPool(threadcount){
			systemThreads=std::thread::hardware_concurrency();
		}

		void startAsync(void *params=0){//run all threads parallel to the thread which called this
			while(!isdone){}//no new instances of this pool until it's finished
			runningThreads=(threadsInPool>0)?threadsInPool:systemThreads;
			threads=new pooledThread*[runningThreads];

			if(isclass){
				for(int i=0;i<runningThreads;i++){
					threads[i]=new pooledThread(classptr,i,runningThreads,params,classfn);
				}
			}else{
				for(int i=0;i<runningThreads;i++){
					threads[i]=new pooledThread(i,runningThreads,params,fn);
				}
			}

			statusThread=new std::thread(&threadPool::statusChecker,this);
			statusThread->detach();
		}
		void start(void *params=0){//run all threads, but only return once they're all finished
		}
		void stop(){
			for(int i=0;i<runningThreads;i++){
				delete threads[i];
			}
			delete[] threads;
			threads=0;

			delete statusThread;
			statusThread=0;
			isdone=true;
		}

		~threadPool(){
			stop();
		}
	};
}

#endif // THREADPOOL_H_INCLUDED
