/**
TODO:
use SDL2
use c++11 threading
*/
#include <SDL2/SDL.h>
#include "engine/engine.h"
#include "engine/math/quat.h"
#include "engine/math/vec3d.h"
using namespace std;

int main(int argc,char** argv){
	MG::quat q;
	q=q*4;
	q=4*q;

	MG::vec3d v;
	v=v*4;
	v=4*v;
	return 0;
}
