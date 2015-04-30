#ifndef QUAT_H_INCLUDED
#define QUAT_H_INCLUDED

#include "vec3d.h"

namespace MG{
	struct quat{
		union{
			struct{double a,b,c,d;};
			double abcd[4];
		};

		quat():a(0),b(0),c(0),d(0){}
		quat(double a0,double b0,double c0,double d0):a(a0),b(b0),c(c0),d(d0){}
		quat(double t,const vec3d &v);//creates a quatereon from a vector and angle

		void setFromAngleVector(double t,const vec3d &v);

		quat operator +(const quat &q) const;
		quat operator -(const quat &q) const;
		quat operator *(const quat &q) const;
		quat operator /(const quat &q) const;

		quat operator *(double q) const;
		quat operator /(double q) const;

		quat operator *(const vec3d &v) const;

		vec3d applyTo(const vec3d &v) const;

		quat& operator +=(const quat &q);
		quat& operator -=(const quat &q);
		quat& operator *=(const quat &q);
		quat& operator /=(const quat &q);

		quat& operator *=(double q);
		quat& operator /=(double q);

		quat conj() const;
		quat inv() const;

		quat getNormalized() const;
		void normalize();

		///TODO: implement this when you have a matrix class
		//operator matrix() const;
	};

	double abs(const quat &q);
	double abs2(const quat &q);
	double invabs(const quat &q);
	double invabs2(const quat &q);
	quat conj(const quat &q);
}
//These are outside of the namespace because otherwise it refuses to compile
MG::quat operator *(double a,const MG::quat &q);
MG::quat operator /(double a,const MG::quat &q);
MG::quat operator *(const MG::vec3d &v,const MG::quat &q);
#endif // QUAT_H_INCLUDED
