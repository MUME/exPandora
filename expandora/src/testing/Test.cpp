#include <stdio.h>
#include "ObjectRecycler.h"
#include "ParseEvent.h"

class parent {
	public:
		parent();
		void pr();
	private:
		char p;
};

class child : public parent {
	public:
		child();
	private:
		char p;
};

parent::parent() {
	p = 'p';
}

child::child() {
	p = 'c';
}

void parent::pr() {
	printf("character: %c\n", p);
}

int main(int argc, char **argv) {
	ParseEvent * event;
	for(int i = 0; i< 10; i++) {
		event = new ParseEvent();
		printf("created event: %i", event);
		ParseEvent.mm.deactivate(event);
		printf("deactivated event: %i", event);
	}
	for (int i = 0; i < 12; i++) {
		event = ParseEvent.mm.activate();
		printf("activated event: %i", event);
	}
	return 0;
}
