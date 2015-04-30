#include "vec3d.h"
#include <cmath>
using namespace MG;

vec3d vec3d::operator +(const vec3d &v) const{
	return vec3d(x+v.x,y+v.y,z+v.z);
}
vec3d vec3d::operator -(const vec3d &v) const{
	return vec3d(x-v.x,y-v.y,z-v.z);
}

vec3d& vec3d::operator =(const vec3d &v){
	x=v.x;
	y=v.y;
	z=v.z;
	return *this;
}
vec3d& vec3d::operator +=(const vec3d &v){
	x+=v.x;
	y+=v.y;
	z+=v.z;
	return *this;
}
vec3d& vec3d::operator -=(const vec3d &v){
	x-=v.x;
	y-=v.y;
	z-=v.z;
	return *this;
}

double vec3d::dot(const vec3d &v) const{
	return x*v.x+y*v.y+z*v.z;
}
vec3d vec3d::cross(const vec3d &v) const{
	return vec3d(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x);
}

vec3d vec3d::operator *(double a) const{
	return vec3d(x*a,y*a,z*a);
}
vec3d vec3d::operator /(double a) const{
	return vec3d(x/a,y/a,z/a);
}
vec3d& vec3d::operator *=(double a){
	x*=a;
	y*=a;
	z*=a;
	return *this;
}
vec3d& vec3d::operator /=(double a){
	x/=a;
	y/=a;
	z/=a;
	return *this;
}

void vec3d::normalize(){
	const double d=1/sqrt(x*x+y*y);
	x*=d;
	y*=d;
	z*=d;
}
vec3d vec3d::getNormalized(){
	const double d=1/sqrt(x*x+y*y+z*z);
	return vec3d(x*d,y*d,z*d);
}

vec3d operator *(double a,const vec3d &v){
	return vec3d(v.x*a,v.y*a,v.z*a);
}

double abs(const vec3d &v){//returns the magnitude
	return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);//old method
}
double abs2(const vec3d &v){//returns the magnitude squared
	return v.x*v.x+v.y*v.y+v.z*v.z;
}
double invabs(const vec3d &v){
	return 1/sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}
