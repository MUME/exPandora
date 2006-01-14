#include <iostream>
#include "Component.h"

using namespace std;


void Component::start() {
	if (thread) {
		moveToThread(thread);
		thread->start();
	}
}

map<QString, ComponentCreator *> & ComponentCreator::creators() {
      static map<QString, ComponentCreator *> creators;
      return creators;
}

Component::Component(bool threaded) {
	if (threaded) {
	  thread = new ComponentThreader;
	}
	else thread = 0;
}

Component::~Component() {
	if(thread) {
		// last chance to get rid of the thread. 
		// components should handle that in their own destructors
		thread->quit();
		delete thread;
	}
}

void ComponentThreader::run() {
  try {
    exec();
  } catch (char const * error) {
	    cerr << error << endl;
	    throw error;
  }
}
