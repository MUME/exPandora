#include "Parser.h"
#include "defines.h"
#include "struct.h"
#include "RoomAdmin.h"
#include "LexDefs.h"
#include "RoomCollection.h"

Parser parser;

Parser::Parser() {
  mostLikelyRoom = 0;
  state = SYNCING;
}

void Parser::setTerrain(Property * ter) {
	
  activeTerrain = (terrains.find((*ter)[0]))->second; // the first character has to be the terrain id
}

void Parser::dropNote(ParseEvent * note) {
  if (mostLikelyRoom != 0) { 
    while (note->next() != 0) mostLikelyRoom->addOptional(note->current());
  }
  else note->recycleProperties();
  pemm.deactivate(note);
}

void Parser::event(BaseEvent * ev) {
  if (ev->type >= 0) {
    // a move event
    playerEvents.push(ev);
  }
	
  else {
		
    switch (ev->type) {
    case ROOM:
    case MOVE_FAIL:
      mudEvents.push(ev);
      break;
    case UNIQUE:
      playerEvents.push(ev);
      break;
    case NOTE:
      // goes into mostLikelyRoom and isn't pushed on the queue, so that the queuePop won't get confused
      dropNote((ParseEvent *)ev);
      return;
    }
  }
  checkQueues();
}       

void Parser::checkQueues() {
  if (mudEvents.empty()) return;
  if (playerEvents.empty()) {
    if (state != SYNCING) state = SYNCING;
  }
  else while ( (!(mudEvents.empty())) && mudEvents.front()->timestamp < playerEvents.front()->timestamp) {
    mudPop();
    state = SYNCING;
  }
  if (mudEvents.empty()) return;
	
  //now we are sure we have a user event that happened before the mud event
	

  switch (state) {
  case APPROVED:
    approved();
    break;
  case EXPERIMENTING:
    experimenting();
    break;
  case SYNCING:
    syncing();
    break;
  }
  toggle_renderer_reaction();
  engine_flags.redraw  = 0;
}	


void Parser::approved() {
  if (playerEvents.front()->type == UNIQUE) {
    mostLikelyRoom->setUnique();
    playerPop();
    return;
  }
  if (mudEvents.front()->type == MOVE_FAIL) {
    mudPop();
    playerPop();
    return;
  }
	
  // now we have a move and a room on the event queues;
  Room * perhaps = 0;
  RoomCollection * possible = mostLikelyRoom->go(playerEvents.front());
  if (possible->numRooms() == 0) {
    Coordinate * c = getExpectedCoordinate();
    perhaps = roomAdmin.getRoom(c);
    if ((perhaps == 0) || (!perhaps->fastCompare((ParseEvent *)mudEvents.front()))) {
      perhaps = roomAdmin.insertRoom((ParseEvent *)mudEvents.front(), c, activeTerrain);
      if (perhaps == 0) {
	// can't insert it for some reason - for example skipped props
	state = SYNCING;
	playerPop();
	mudPop();
	cmm.deactivate(c);
	rcmm.deactivate(possible);
	return;
      }
      else {	
	possible->merge(perhaps->getHome());
	state = EXPERIMENTING;
	cmm.deactivate(c);
      }
    }
    else {
      mostLikelyRoom->addExit(playerEvents.front()->type, perhaps);
      mostLikelyRoom = perhaps;
      playerPop();
      mudPop();
      cmm.deactivate(c);
      rcmm.deactivate(possible);
      return;
    }
  }
  else {
	
    if ((perhaps = possible->matchOne((ParseEvent *)mudEvents.front())) != 0) { // narrows possible by the event and return a Room if exactly one is left
      mostLikelyRoom = perhaps;
      mudPop();
      playerPop();
      rcmm.deactivate(possible);
      return;
    }
    else {
      Coordinate * c = getExpectedCoordinate();
      possible->merge(roomAdmin.insertRoom((ParseEvent *)mudEvents.front(), c, activeTerrain)->getHome());
      state = EXPERIMENTING;
      cmm.deactivate(c);
    }
  }
  buildPaths(possible);
  rcmm.deactivate(possible);
}

void Parser::playerPop() {
  if (playerEvents.front()->type >= 0) bemm.deactivate(playerEvents.front());
  else bemm.deactivate(playerEvents.front());
  playerEvents.pop();
}
			
void Parser::mudPop() {
  if (mudEvents.front()->type >= 0) pemm.deactivate((ParseEvent *)mudEvents.front());
  else bemm.deactivate(mudEvents.front());
  mudEvents.pop();
}

void Parser::syncing() {
  if (mudEvents.empty()) return;
  if (!(playerEvents.empty()) && (playerEvents.front()->type == UNIQUE)) {
    // unique doesn't work when syncing ...
    playerPop();
    return;
  }
  if (mudEvents.front()->type == MOVE_FAIL) {
    mudPop();
    if (!playerEvents.empty()) playerPop();
    return;
  }
	
  // now we have a move and a room on the event queues;
	
  RoomSearchNode * possible = roomAdmin.getRooms((ParseEvent *)mudEvents.front());
 
  if (possible->numRooms() > 0) {
    if (mostLikelyRoom == 0) {
      if (possible->numRooms() == 1) {
	state = EXPERIMENTING;
	mostLikelyRoom = *(((RoomCollection *)possible)->begin());
      }
    }
    else {
      state = EXPERIMENTING;
      buildPaths((RoomCollection *)possible);
    }
    rcmm.deactivate((RoomCollection *)possible);
  }
  else if (possible->numRooms() == 0)
    rcmm.deactivate((RoomCollection *)possible);

  if (!(playerEvents.empty())) playerPop();
  mudPop();

}	

void Parser::unify() {
  state = APPROVED;
  mostLikelyRoom = paths.front()->getRoom();
  paths.front()->approve();
  list<Path *>::iterator i = paths.begin()++;
  for (; i != paths.end(); i++) {
    (*i)->deny();
  }
  paths.clear();
}

void Parser::experimenting() {
  if (playerEvents.front()->type == UNIQUE) {
    unify();
    playerPop();
    return;
  }
  if (mudEvents.front()->type == MOVE_FAIL) {
    mudPop();
    playerPop();
    return;
  }
	
  RoomSearchNode * possible = roomAdmin.getRooms((ParseEvent *)mudEvents.front());
  if (possible->numRooms() == -1) possible = rcmm.activate();
  enlargePaths((RoomCollection *)possible, true);
  rcmm.deactivate((RoomCollection *) possible);
  playerPop();
  mudPop();
}

void Parser::buildPaths(RoomCollection * rc) {
  Path * working = pamm.activate();
  working->init(mostLikelyRoom);
  paths.push_front(working);
  enlargePaths(rc, false);
}

void Parser::enlargePaths(RoomCollection * rc, bool includeNew) {
  list<Path *>::iterator i = paths.begin();
  set<Room *>::iterator j = rc->begin();
  ParseEvent * copy = pemm.activate();
  Coordinate * c = cmm.activate();
  Path * working;
  for (; i != paths.end(); i++) {
    for (; j != rc->end(); j++) {
      working = (*i)->fork(*j);
      if (working->getProb() < MINPROB) working->deny();
      else {
	if (working->getProb() > paths.front()->getProb()) paths.push_front(working);
	else paths.insert(i, working);
      }
    }
    if (includeNew) {
      c->clear();
      copy->clear();
      copy->copy((ParseEvent *)mudEvents.front());
      c->add((*i)->getRoom()->getCoordinate());
      c->add(stdMoves[playerEvents.front()->type]);
      working = (*i)->fork(roomAdmin.quickInsert(copy, c, activeTerrain));
      if (working->getProb() < MINPROB) working->deny();
      else {
	if (working->getProb() > paths.front()->getProb())
	  paths.push_front(working);
	else 
	  paths.insert(i, working); 
      }
    }
    if (!(*i)->hasChildren()) (*i)->deny();		
  }
  cmm.deactivate(c);
  pemm.deactivate(copy);
  if (paths.empty()) state = SYNCING;
  else  {
    mostLikelyRoom = paths.front()->getRoom();
    if (paths.begin()++ == paths.end()) { // excactly one path left -> go APPROVED
      paths.front()->approve();
      paths.clear();
      state = APPROVED;
      return;
    }
  }
}



Coordinate * Parser::getExpectedCoordinate() {
  Coordinate * c = cmm.activate();
  c->add(stdMoves[playerEvents.front()->type]);
  c->add(mostLikelyRoom->getCoordinate());
  return c;
}

