#ifndef DEFAULTLIGHTS_H_INCLUDED
#define DEFAULTLIGHTS_H_INCLUDED

#include "lightsource.h"

namespace MG{
	class pointlight:public light{
	public:
		virtual void getRays(const int n,ray* target) const;
	};
}

#endif // DEFAULTLIGHTS_H_INCLUDED
