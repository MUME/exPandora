#include <iostream>
#include "Component.h"

using namespace std;




map<QString, ComponentCreator *> & ComponentCreator::creators() {
      static map<QString, ComponentCreator *> creators;
      return creators;
}

Component::Component(bool threaded) {
	if (threaded) {
	  thread = new ComponentThreader(this);
	}
	else {
	  thread = 0;
	}
}

void Component::start() {
  if (thread) {
    moveToThread(thread);
    thread->start();
  }
  else 
    init();
}

Component::~Component() {
	if(thread) {
		// last chance to get rid of the thread. 
		// components should handle that in their own destructors
		thread->terminate();
		delete thread;
	}
}

void ComponentThreader::run() {
  try {
    owner->runInit();
    exec();
  } catch (char const * error) {
	    cerr << error << endl;
	    throw error;
  }
}
