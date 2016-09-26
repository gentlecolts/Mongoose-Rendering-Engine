#ifndef META_H_INCLUDED
#define META_H_INCLUDED

#include <string>

namespace MG{
	class obj;
	struct metadata{
		std::string name;
		int id;
		obj *object;
	};
}
#endif // META_H_INCLUDED
