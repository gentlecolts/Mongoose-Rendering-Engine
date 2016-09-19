#include "demos.h"
#include "stdio.h"
using namespace std;

//void test1(int index,int threads,void *teststuff){
void test1(int index,int threads,void *teststuff){
	printf("function thread #%i/%i and the magic number is %i\n",index,threads,*(int*)teststuff);
}

class testclass{
private:
	int a;
	void testfn(int index,int threads,void *teststuff){
	}
public:
	testclass():a(rand()){
		MG::threadPool<testclass> classtest(this,&testclass::testfn);
	}
};

void demos::threadPoolTest(){
	printf("running function test\n");
	MG::threadPool<> fntest(test1,2);
	int magicnum=5;
	fntest.startAsync(&magicnum);
	printf("function's threads started\n");

	while(!fntest.isdone){}
	printf("finished!");
}
