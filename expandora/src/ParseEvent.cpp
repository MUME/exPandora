
#include "ParseEvent.h"
#include "defines.h"
#include "utils.h"

ObjectRecycler<ParseEvent> pemm;

ParseEvent::ParseEvent() {
  attachList(&required);
  type = 0;
  timestamp = 0;
  tos = 0;
  opTos = 0;
}

void ParseEvent::clear() {
  recycleProperties();
  type = 0;
  timestamp = 0;
  pos = required.size();
  tos = 0;
  opTos = 0;
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
  for (unsigned int p = 0; otherProps->get(p) != 0; p++) {
    prop = otherProps->get(p)->copy();
    required.put(p, prop);
  }
  otherProps = other->getOptionals();

  for (unsigned int p = 0; otherProps->get(p) != 0; p++) {
    prop = otherProps->get(p)->copy();
    optionals.put(p, prop);
  }
}


/**
 * give the properties back to the recycler - should only be done if they aren't needed in a newly created room
 */
void ParseEvent::recycleProperties() {
  for (unsigned int p = 0; required.get(p) != 0; p++) {
    pmm.deactivate(required.get(p));
    required.remove(p);
  }
  for (unsigned int p = 0; optionals.get(p) != 0; p++) {
    pmm.deactivate(optionals.get(p));
    optionals.remove(p);
  }
}

void ParseEvent::push(Property * newProp) {
  required.put(tos++, newProp);
  pos = required.size();
  if (timestamp == 0) timestamp = m_timestamp();
}


	

void ParseEvent::pushOptional(Property * newProp) {
  optionals.put(opTos++, newProp);
  if (timestamp == 0) timestamp = m_timestamp();
}




