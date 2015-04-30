#ifndef VEC3D_H_INCLUDED
#define VEC3D_H_INCLUDED
/*
This header outlines the struct vec3d
the vec3d struct provides many commontly used operators for 3d vectors
*/
namespace MG{
	struct vec3d{
		/*
		this union allows for a user to easily access the points as an array for efficency
		or as names for readability as the user chooses;
		*/
		union{
			struct{double x,y,z;};
			double xyz[3];
		};

		vec3d():x(0),y(0),z(0){}
		vec3d(double x0,double y0,double z0):x(x0),y(y0),z(z0){}

		vec3d operator +(const vec3d &v) const;
		vec3d operator -(const vec3d &v) const;
		vec3d& operator =(const vec3d &v);
		vec3d& operator +=(const vec3d &v);
		vec3d& operator -=(const vec3d &v);

		vec3d operator *(double a) const;
		vec3d operator /(double a) const;
		vec3d& operator *=(double a);
		vec3d& operator /=(double a);

		void normalize();
		vec3d getNormalized();

		double dot(const vec3d &v) const;
		vec3d cross(const vec3d &v) const;
	};

	double abs(const vec3d &v);//returns the magnitude
	double abs2(const vec3d &v);//returns the magnitude squared
	double invabs(const vec3d &v);//returns 1/magnitude, reason for inclusion being common use and potential (hardware) optimization
	double invabs2(const vec3d &v);//returns 1/magnitude^2
}
//this is outside of the namespace because otherwise things refuse to compile
MG::vec3d operator *(double a,const MG::vec3d &v);
#endif // VEC3D_H_INCLUDED
