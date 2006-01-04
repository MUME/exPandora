#include "Component.h"

/*void Component::run() {
	std::cout << "this component is not threaded and doesn't need starting"<< std::endl;
}*/

void Component::start() {
	if (thread) {
		thread->start();
	}
}


Component::Component(bool threaded) {
	if (threaded) thread = new ComponentThreader(this);
	else thread = 0;
}

Component::~Component() {
	if(thread) {
		// last chance to get rid of the thread. 
		// components should handle that in their own destructors
		thread->terminate();
		delete thread;
	}
}
