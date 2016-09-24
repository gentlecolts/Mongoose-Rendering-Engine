#include "demos.h"
#include "stdio.h"
using namespace std;

void test1(int index,int threads,void *teststuff){
	printf("function thread #%i/%i and the magic number is %i\n",index,threads,*(int*)teststuff);
}

void test2(int index,int threads,void *teststuff){
	loop:
		printf("i'm an evil infinite loop in thread #%i/%i\n",index,threads);
	goto loop;
}

void test3(int index,int threads){
	printf("this thread has no extra args %i/%i\n",index,threads);
}

void test4(int index,int threads,int a,int b){
	printf("this thread has many extra args %i/%i: %i, %i\n",index,threads,a,b);
}

class testclass{
private:
	int a;
	void testfn(int index,int threads,float f){
		printf("class function thread #%i/%i and the magic numbers are %i and %f\n",index,threads,a,f);
	}
public:
	testclass():a(rand()){
		printf("hi, i'm the class fn and inline start tester\n");
		MG::threadPool classtest(&testclass::testfn,-1,true,this,3.6f);
		printf("finished the class thread\n");
	}
};

void demos::threadPoolTest(){
	printf("running function test\n");
	int magicnum=5;
	MG::threadPool fntest(&test1,2,false,(void*)&magicnum);
	printf("function's threads started\n");

	//while(!fntest.isdone){}
	fntest.finish();
	printf("finished!\n");

	//MG::threadPool<> looptester(test2,2);
	//looptester.startAsync();

	printf("starting the class\n");
	testclass classtester;

	printf("class has finished\n");
	//printf("attempting to kill inf loop\n");
	//looptester.finish();//will never finish

	printf("testing the arg functions\n");
	MG::threadPool noargs(test3,-1,true);
	MG::threadPool moreargs(test4,-1,true,2,7);

	printf("and we're done!\n");
}
