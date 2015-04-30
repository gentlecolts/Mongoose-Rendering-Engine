#include "quat.h"
#include <cmath>
using namespace MG;

quat::quat(double t,const vec3d &v){
	setFromAngleVector(t,v);
}

void quat::setFromAngleVector(double t,const vec3d &v){
	const double s=sin(t/2);
	a=cos(t/2);
	b=v.x*s;
	c=v.y*s;
	d=v.z*s;
	//normalize();
}

quat quat::operator +(const quat &q) const{
	return quat(a+q.a,b+q.b,c+q.c,d+q.d);
}
quat quat::operator -(const quat &q) const{
	return quat(a-q.a,b-q.b,c-q.c,d-q.d);
}
quat quat::operator *(const quat &q) const{
	return quat(
			a*q.a-b*q.b-c*q.c-d*q.d,
			a*q.b+b*q.a+c*q.d-d*q.c,
			a*q.c-b*q.d+c*q.a+d*q.b,
			a*q.d+b*q.c-c*q.b+d*q.a);
}
quat quat::operator /(const quat &q) const{
	return (*this)*q.inv();
}

quat quat::operator *(double q) const{
	return quat(a*q,b*q,c*q,d*q);
}
quat quat::operator /(double q) const{
	return quat(a/q,b/q,c/q,d/q);
}

quat quat::operator *(const vec3d &v) const{
	return quat(
			-b*v.x-c*v.y-d*v.z,
			a*v.x+c*v.z-d*v.y,
			a*v.y-b*v.z+d*v.x,
			a*v.z+b*v.y-c*v.x);
}
quat operator *(const vec3d &v,const quat &q){
	return quat(
			-v.x*q.b-v.y*q.c-v.z*q.d,
			v.x*q.a+v.y*q.d-v.z*q.c,
			-v.x*q.d+v.y*q.a+v.z*q.b,
			v.x*q.c-v.y*q.b+v.z*q.a);
}

vec3d quat::applyTo(const vec3d &v) const{
	return vec3d(
		(a*a+b*b-c*c-d*d)*v.x + 2*((b*c - a*d)*v.y + (a*c + b*d)*v.z),
		(a*a-b*b+c*c-d*d)*v.y + 2*((c*d - a*b)*v.z + (a*d + b*c)*v.x),
		(a*a-b*b-c*c+d*d)*v.z + 2*((b*d - a*c)*v.x + (a*b + c*d)*v.y)
	);
}

quat& quat::operator +=(const quat &q){
	a+=q.a;
	b+=q.b;
	c+=q.c;
	d+=q.d;
	return *this;
}
quat& quat::operator -=(const quat &q){
	a-=q.a;
	b-=q.b;
	c-=q.c;
	d-=q.d;
	return *this;
}
quat& quat::operator *=(const quat &q){
	return *this=(*this)*q;
}
quat& quat::operator /=(const quat &q){
	return *this=(*this)*q.inv();
}

quat& quat::operator *=(double q){
	a*=q;
	b*=q;
	c*=q;
	d*=q;
	return *this;
}
quat& quat::operator /=(double q){
	a/=q;
	b/=q;
	c/=q;
	d/=q;
	return *this;
}

quat quat::conj() const{
	return quat(a,-b,-c,-d);
}
quat quat::inv() const{
	const double q=1/(a*a+b*b+c*c+d*d);
	return quat(a*q,-b*q,-c*q,-d*q);//note that this is conj(q)/|q|^2
}

quat quat::getNormalized() const{
	const double q=1/sqrt(a*a+b*b+c*c+d*d);
	return quat(a*q,b*q,c*q,d*q);
}
void quat::normalize(){
	const double q=1/sqrt(a*a+b*b+c*c+d*d);
	a*=q;
	b*=q;
	c*=q;
	d*=q;
}

/**TODO: create an actual matrix class that this can cast to
quat::operator matrix() const{
	mtype tmp[9]={
		a*a+b*b-c*c-d*d,2*(b*c-a*d),2*(b*d+a*c),
		2*(b*c+a*d),a*a-b*b+c*c-d*d,2*(c*d-a*b),
		2*(b*d-a*c),2*(c*d+a*b),a*a-b*b-c*c+d*d};
	return matrix(3,3,tmp);
}
*/

double abs(const quat &q){
	return sqrt(q.a*q.a+q.b*q.b+q.c*q.c+q.d*q.d);
}
double abs2(const quat &q){
	return q.a*q.a+q.b*q.b+q.c*q.c+q.d*q.d;
}
double invabs(const quat &q){
	return 1/sqrt(q.a*q.a+q.b*q.b+q.c*q.c+q.d*q.d);
}
double invabs2(const quat &q){
	return 1/(q.a*q.a+q.b*q.b+q.c*q.c+q.d*q.d);
}

quat conj(const quat &q){
	return q.conj();
}

quat operator *(double a,const quat &q){
	return q*a;
}
quat operator /(double a,const quat &q){
	return q.inv()*a;
}
