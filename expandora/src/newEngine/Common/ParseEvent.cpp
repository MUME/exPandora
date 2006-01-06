#include "ParseEvent.h"
#include <math.h>



ObjectRecycler<ParseEvent> pemm;

ParseEvent::ParseEvent() {
  attachList(&required);
  reqStack.attachList(&required);
  optStack.attachList(&optionals);
  type = 0;
  timestamp = 0;
}

void ParseEvent::clear() {
  recycleProperties();
  type = 0;
  timestamp = 0;
  pos = required.size();
}

ParseEvent * ParseEvent::copy() {
  ParseEvent * p = pemm.activate();
  p->copy(this);
  return p;
}

void ParseEvent::reset() {
  ListCycler<Property *>::reset();
  for (unsigned int i = 0; required.get(i) != 0; i++) required.get(i)->reset();
  for (unsigned int i = 0; optionals.get(i) != 0; i++) optionals.get(i)->reset();

}

void ParseEvent::copy(ParseEvent * other) {
  timestamp = other->timestamp;
  type = other->type;
  pos = other->getPos();
  TinyList<Property *> * otherProps = other->getProperties();
  Property * prop = 0;
  for (unsigned int p = 0; otherProps->get(p) != 0; ++p) {
    prop = otherProps->get(p)->copy();
    reqStack.push(prop);
  }
  otherProps = other->getOptionals();

  for (unsigned int p = 0; otherProps->get(p) != 0; ++p) {
    prop = otherProps->get(p)->copy();
    optStack.push(prop);
  }
}


/**
 * give the properties back to the recycler - should only be done if they aren't needed in a newly created room
 */
void ParseEvent::recycleProperties() {
  for (Property * p = reqStack.pop(); p != 0; p = reqStack.pop()) 
    pmm.deactivate(p);

  for (Property * p = optStack.pop(); p != 0; p = optStack.pop()) 
    pmm.deactivate(p);
  
  pos = required.size();
}

void ParseEvent::push(Property * newProp) {
  reqStack.push(newProp);
  pos = required.size();
}


	

void ParseEvent::pushOptional(Property * newProp) {
  optStack.push(newProp);
}





