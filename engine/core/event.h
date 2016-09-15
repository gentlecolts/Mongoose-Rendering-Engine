#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED
#include "core.h"

namespace MG{
	class event{
	protected:
		virtual void quit();//called when main engine object is destroyed
		virtual void keyDown(char key);
		virtual void keyUp(char key);
	public:
		event();
	};
}
#endif // EVENT_H_INCLUDED
