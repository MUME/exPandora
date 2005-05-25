//#include <qstring.h>
//#include "../SearchTreeNode.h"
//#include "../Room.h"
#include <iostream>
#include <qmetaobject.h>
using namespace std;

#include "Test.h"

void Baum::baumel(int, int) {cout << "baumel\n";}
void Baum::semmel(char * in) {cout << "semmel" << in << "\n";}
void Baum::go() {
  emit gurke("hannes");
} 

int main(int argc, char **argv) {
  
  Baum baum;
  int i = baum.metaObject()->numSlots();
  cout << i;
  cout << "baum\n";
  QStrList l = baum.metaObject()->signalNames(); //->signalNames();
  QStrList m = baum.metaObject()->slotNames(); //->signalNames();
  for (int j = 0; j != i; ++j) {
    cout << l.at(j) << "\n";
  }
  cout << SIGNAL(gurke(char *)) << "\n";
  cout << SLOT(semmel(char *)) << "\n";
  QObject::connect(&baum, SIGNAL(gurke(char *)), &baum, SLOT(semmel(char *)));
  baum.go();

  //cout << baum.metaObject()->signalNames(); //->slotNames();

//SearchTreeNode * test = new SearchTreeNode("baum");

/*
	char * string = new char[128];
	string[127] = 0;
	TinyList<char> l;
	for (char i = 127; i >= 0; i--) {
		string[i] = i+1;
		l.put(i, strdup(string));
		printf("put at %i: %s\n", i, string);
	}
	for (char i = 0; i < 128; i++) {
		printf("found at %i: %s\n", i, l.get(i));
	}	
	ParseEvent * event;
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
	}
	printf("ParseEvent: %i", sizeof(ParseEvent));
	printf("Property: %i", sizeof(Property));
	printf("QString: %i", sizeof(QString));
	*/
	return 0;
}
