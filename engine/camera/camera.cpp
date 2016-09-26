#include "camera.h"
using namespace MG;

camera::camera(){
}
camera::camera(surface* renderTarget):target(renderTarget){
}
camera::camera(scene* s):sceneToDraw(s){
}
camera::camera(surface* renderTarget,scene* s):target(renderTarget),sceneToDraw(s){
}
