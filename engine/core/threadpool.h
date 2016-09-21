#ifndef THREADPOOL_H_INCLUDED
#define THREADPOOL_H_INCLUDED

#include <thread>

namespace MG{
	class empty{};
	template<typename T=empty>
	class threadPool{
	private:
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
			}
			pooledThread(T* ptr,int index,int totalThreads,void* params,void (T::*pfunc)(int,int,void*)){
				mythread=new std::thread(pfunc,ptr,index,totalThreads,params);
			}

			~pooledThread(){
				//printf("got it, i'm stopping\n");
				/**TODO: fix this?
				if(mythread->joinable()){
					//printf("was joinable, is not anymore\n");
					mythread->detach();//detach the thread so it can be stopped mid-execution
					//mythread->join();//just hope that this finishes
				}//*/
				delete mythread;
				mythread=0;
			}
		};

		pooledThread** threads=0;
		int runningThreads=0;

		std::thread* statusThread=0;

		void statusChecker(){
			for(int i=0;i<runningThreads;i++){
				threads[i]->mythread->join();
			}
			finish();
			isdone=true;
		}
		void cleanStatusThread(){
			if(statusThread){
				//printf("status thread not free, killing\n");
				statusThread->join();
				delete statusThread;
				statusThread=0;
			}
		}
	public:
		int threadsInPool;
		bool isdone=true;
		threadPool(void (*pfunc)(int,int,void*),int threadcount=-1):isclass(false),fn(pfunc),threadsInPool(threadcount){
			systemThreads=std::thread::hardware_concurrency();
			//printf("we've got %i system threads\n",systemThreads);
		}
		threadPool(T* instance,void (T::*pfunc)(int,int,void*),int threadcount=-1):isclass(true),classptr(instance),classfn(pfunc),threadsInPool(threadcount){
			systemThreads=std::thread::hardware_concurrency();
		}

		void startAsync(void *params=0){//run all threads parallel to the thread which called this
			//printf("starting async\n");
			while(!isdone){}//no new instances of this pool until it's finished
			cleanStatusThread();//clean up the status thread from any potential past runs
			isdone=false;
			//printf("any past instances have finished running\n");
			runningThreads=(threadsInPool>0)?threadsInPool:systemThreads;

			//if(runningThreads<1){isdone=true;return;}//above line makes this irrelevant

			threads=new pooledThread*[runningThreads];

			//printf("we're running %i threads\n",runningThreads);

			if(isclass){
				//printf("we're using a class function\n");
				for(int i=0;i<runningThreads;i++){
					//printf("making thread %i\n",i);
					threads[i]=new pooledThread(classptr,i,runningThreads,params,classfn);
					//printf("done\n");
				}
			}else{
				//printf("we're using a normal function\n");
				for(int i=0;i<runningThreads;i++){
					//printf("making thread %i\n",i);
					threads[i]=new pooledThread(i,runningThreads,params,fn);
					//printf("done\n");
				}
			}

			statusThread=new std::thread(&threadPool::statusChecker,this);
			//statusThread->detach();
		}
		void start(void *params=0){//run all threads, but only return once they're all finished
			//printf("starting inline\n");
			startAsync(params);
			//printf("waiting for async to finish\n");
			statusChecker();
			//printf("static pool finished\n");
		}

		~threadPool(){
			/*dont do anything until we've completely finished
			TODO: figure out how to terminate threads without crashing so this can be removed*/
			while(!isdone){}
			//printf("thread pool is deleting\n");
			finish();
			//printf("any leftover threads have been stopped and freed\n");
			cleanStatusThread();
			//printf("done with pool cleanup\n");
		}

		void finish(){
			/*wait for the threads to finish before clearing them
			note that this will loop forever if any thread is an infinite loop
			TODO: is there a better way to do this?*/
			while(!isdone){}

			if(threads){
				//printf("stopping the pool\n");
				for(int i=0;i<runningThreads;i++){
					if(threads[i]){delete threads[i];}
				}
				delete[] threads;
				threads=0;
			}

			/*this is removed because the status checker needs to call stop for cleanup purposes
			delete statusThread;
			statusThread=0;//*/
			isdone=true;
			//printf("pool stopped\n");
		}
	};
}

#endif // THREADPOOL_H_INCLUDED
