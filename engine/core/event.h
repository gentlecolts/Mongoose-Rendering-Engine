#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

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
		virtual void keyDown(char key){}
		virtual void keyUp(char key){}
	};
}

#endif // EVENT_H_INCLUDED
