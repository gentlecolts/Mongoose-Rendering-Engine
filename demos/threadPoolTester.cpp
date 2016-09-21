#include "demos.h"
#include "stdio.h"
using namespace std;

void test1(int index,int threads,void *teststuff){
	printf("function thread #%i/%i and the magic number is %i\n",index,threads,*(int*)teststuff);
}

void test2(int index,int threads,void *teststuff){
	loop:
		printf("i'm an evil infinite loop in thread #%i/%i\n",index,threads);
	//goto loop;
}

class testclass{
private:
	int a;
	void testfn(int index,int threads,void *teststuff){
		printf("class function thread #%i/%i and the magic number is %i\n",index,threads,a);
	}
public:
	testclass():a(rand()){
		printf("hi, i'm the class fn and inline start tester\n");
		MG::threadPool<testclass> classtest(this,&testclass::testfn);
		classtest.start();
		printf("finished the class thread\n");
	}
};

void demos::threadPoolTest(){
	printf("running function test\n");
	MG::threadPool<> fntest(test1,2);
	int magicnum=5;
	fntest.startAsync(&magicnum);
	printf("function's threads started\n");

	while(!fntest.isdone){}
	printf("finished!\n");

	//MG::threadPool<> looptester(test2,2);
	//looptester.startAsync();

	printf("starting the class\n");
	testclass classtester();

	printf("class has finished, killing the inf loop\n");
	//looptester.stop();
	printf("and we're done!\n");
}
