#include <qstring.h>
#include <stdio.h>
#include "../ParseEvent.h"


int main(int argc, char **argv) {
	/*ParseEvent * event;
	ObjectRecycler<ParseEvent> mm;
	for(int i = 0; i< 10; i++) {
		event = new ParseEvent();
		printf("created event: %i\n", event);
		mm.deactivate(event);
		printf("deactivated event: %i\n", event);
	}
	printf("\n\nnext turn ...\n\n");
	for (int i = 0; i < 12; i++) {
		event = mm.activate();
		//event = event->mm.activate();
		printf("activated event: %i\n", event);
	}*/
	printf("ParseEvent: %i", sizeof(ParseEvent));
	printf("Property: %i", sizeof(Property));
	printf("QString: %i", sizeof(QString));
	return 0;
}
