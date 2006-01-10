#include "Component.h"
#include <iostream>
using namespace std;


void Component::start() {
	if (thread) {
	  try {
		moveToThread(thread);
		thread->start();
	  } catch (char const * error) {
	    cerr << error << endl;
	    thread->terminate();
	    delete thread;
	  }
		
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
