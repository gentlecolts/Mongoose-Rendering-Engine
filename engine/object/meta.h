#ifndef META_H_INCLUDED
#define META_H_INCLUDED

#include "object.h"
#include <string>

namespace MG{
	struct meta{
		std::string name;
		int id;
		obj *object;
	};
}
#endif // META_H_INCLUDED
