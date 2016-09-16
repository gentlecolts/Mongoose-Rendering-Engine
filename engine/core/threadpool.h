#ifndef THREADPOOL_H_INCLUDED
#define THREADPOOL_H_INCLUDED

#include <thread>

namespace MG{
	class pooledThread{
		int id,threadsInPool;
		void (*fn)(int,int,void*);//id, threads in pool, and arbitrary parenters
	public:
		bool isrunning,isdone;
		pooledThread(int index,int totalThreads,void (*pfunc)(int,int,void*)):id(index),threadsInPool(totalThreads),fn(pfunc){}

		void start();
		void stop();
	};

	class threadPool{
		int systemThreads;
		void (*fn)(int,int,void*);
	public:
		int threadsInPool;
		threadPool(void (*pfunc)(int,int,void*),int threadcount=-1);
	};
}

#endif // THREADPOOL_H_INCLUDED
