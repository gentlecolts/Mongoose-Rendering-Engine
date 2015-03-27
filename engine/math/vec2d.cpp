#include "vec2d.h"
#include <cmath>
using namespace MG;

vec2d vec2d::operator +(vec2d &v) const{
	return vec2d(x+v.x,y+v.y);
}
vec2d vec2d::operator -(vec2d &v) const{
	return vec2d(x-v.x,y-v.y);
}

vec2d& vec2d::operator =(vec2d &v){
	x=v.x;
	y=v.y;
	return *this;
}
vec2d& vec2d::operator +=(vec2d &v){
	x+=v.x;
	y+=v.y;
	return *this;
}
vec2d& vec2d::operator -=(vec2d &v){
	x-=v.x;
	y-=v.y;
	return *this;
}

double vec2d::dot(vec2d &v) const{
	return x*v.x+y*v.y;
}

vec2d vec2d::operator *(double a) const{
	return vec2d(x*a,y*a);
}
vec2d vec2d::operator /(double a) const{
	return vec2d(x/a,y/a);
}
vec2d& vec2d::operator *=(double a){
}
vec2d& vec2d::operator /=(double a){
}

void vec2d::normalize(){
	const double d=1/sqrt(x*x+y*y);
	x*=d;
	y*=d;
}
vec2d vec2d::getNormalized(){
	const double d=1/sqrt(x*x+y*y);
	return vec2d(x*d,y*d);
}

vec2d operator *(double a,vec2d &v){
	return vec2d(v.x*a,v.y*a);
}

double abs(vec2d &v){//returns the magnitude
	return sqrt(v.x*v.x+v.y*v.y);
}
double abs2(vec2d &v){//returns the magnitude squared
	return v.x*v.x+v.y*v.y;
}
