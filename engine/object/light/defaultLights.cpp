#include "defaultLights.h"
#include <random>
#include <cmath>
using namespace MG;

static const double pi=4*std::atan(1.0);
static std::random_device generator;
static std::uniform_real_distribution<double> randAngle(0,2*pi);
static std::uniform_real_distribution<double> randFloat(-1.0,1.0);

void pointlight::getRays(const int n, ray* target) const{

	for(ray* r=target;r<target+n;r++){
		const double
			u=randFloat(generator),
			theta=randAngle(generator),
			rt=std::sqrt(1-u*u);

		r->from=pos;
		r->c=c;
		r->dir=vec3d(rt*std::cos(theta),rt*std::sin(theta),u);
	}
}
