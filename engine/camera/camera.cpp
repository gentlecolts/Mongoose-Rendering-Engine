#include "camera.h"
using namespace MG;

camera::camera(surface* renderTarget,scene* s):camera(){
	target=renderTarget;
	sceneToDraw=s;
}
