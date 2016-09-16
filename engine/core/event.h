#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include <stdint.h>

namespace MG{
	class engine;

	class event{
	protected:
		engine *parent;
		int index;
	public:
		event(engine* engineParent);
		~event();

		virtual void quit(){}
		virtual void keyEvent(bool keyDown,int key){}
		virtual void mouseMoved(int x,int y,int dx,int dy){}
		virtual void mouseButton(bool buttonDown,uint8_t button){}
	};
}

#endif // EVENT_H_INCLUDED
