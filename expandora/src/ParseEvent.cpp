#include "ParseEvent.h"


void ParseEvent::clear() {
	while(!propertyBeginnings.empty()) propertyBeginnings.pop();
	while(!propertyEnds.empty()) propertyEnds.pop();
	type = 0;
}
