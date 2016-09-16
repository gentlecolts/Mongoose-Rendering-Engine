#include "threadpool.h"
using namespace MG;

/////pooledThread functions go here/////


/////threadPool functions go here/////
threadPool::threadPool(void (*pfunc)(int,int,void*),int threadcount):fn(pfunc),threadsInPool(threadcount){
}
