#include "Parser.h"
#include "defines.h"
#include "struct.h"
#include "RoomAdmin.h"
#include "LexDefs.h"
#include "RoomCollection.h"

Parser parser(&roomAdmin);

Parser::Parser(RoomAdmin * _admin) {
  mostLikelyRoom = 0;
  state = SYNCING;
  admin = _admin;
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

void Parser::event(ParseEvent * ev) {
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
    perhaps = admin->getRoom(c);
    if ((perhaps == 0) || (!perhaps->fastCompare((ParseEvent *)mudEvents.front()))) {
      perhaps = admin->insertRoom((ParseEvent *)mudEvents.front(), c, activeTerrain);
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
      possible->merge(admin->insertRoom((ParseEvent *)mudEvents.front(), c, activeTerrain)->getHome());
      state = EXPERIMENTING;
      cmm.deactivate(c);
    }
  }
  buildPaths(possible);
  rcmm.deactivate(possible);
}

void Parser::playerPop() {
  pemm.deactivate(playerEvents.front());
  playerEvents.pop();
}
			
void Parser::mudPop() {
  pemm.deactivate(mudEvents.front());
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
	
  RoomSearchNode * possible = admin->getRooms((ParseEvent *)mudEvents.front());
 
  if (possible->numRooms() > 0) {
    if (mostLikelyRoom == 0) {
      if (possible->numRooms() == 1) {
	state = APPROVED;
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
	
  RoomSearchNode * possible = admin->getRooms((ParseEvent *)mudEvents.front());
  if (possible->numRooms() == -1) possible = rcmm.activate();
  enlargePaths((RoomCollection *)possible, true);
  rcmm.deactivate((RoomCollection *) possible);
  playerPop();
  mudPop();
}

void Parser::buildPaths(RoomCollection * rc) {
  Path * working = pamm.activate();
  working->init(mostLikelyRoom, admin);
  paths.push_front(working);
  enlargePaths(rc, false);
}

void Parser::enlargePaths(RoomCollection * rc, bool includeNew) {
  if (paths.begin() == paths.end()) return;
  list<Path *>::iterator i = paths.begin();
  set<Room *>::iterator j = rc->begin();

  ParseEvent * copy = 0;
  Coordinate * c = 0;
  

  Path * working = 0;
  Path * best = 0;
  double prevBest = paths.front()->getProb();

  int k = paths.size();
  

  for (int l = 0; l < k; l++) {
    
    for (j = rc->begin(); j != rc->end(); j++) {
      working = (*i)->fork(*j);
      if (working->getProb() < prevBest*MINPROB) {
	(*i)->removeChild(working);
	pamm.deactivate(working);
      }
      else {
	if (best == 0) best = working;
	else if(working->getProb() > best->getProb()) {
	  paths.push_back(best);
	  best = working;
	}
	else paths.push_back(working);
      }
    }
    if (includeNew) {
      copy = pemm.activate();
      c = cmm.activate();
      copy->copy((ParseEvent *)mudEvents.front());
      c->add((*i)->getRoom()->getCoordinate());
      c->add(stdMoves[playerEvents.front()->type]);
      working = (*i)->fork(admin->quickInsert(copy, c, activeTerrain));
      working->setProb(working->getProb()/2.0);
      cmm.deactivate(c);
      pemm.deactivate(copy);
      // events are eaten by the room, coordinates not because coordinates can change during the insert process
      // nevertheless we can deactivate the copy because we do a shallow deactivate
      if (working->getProb() < MINPROB*prevBest) {
	(*i)->removeChild(working);
	pamm.deactivate(working);
      }
      else {
	if (best == 0) best = working;
	else if(working->getProb() > best->getProb()) {
	  paths.push_back(best);
	  best = working;
	}
	else paths.push_back(working);
      }
    }
    i++;
  } 
  for (int l = 0; l < k; l++) {
    working = paths.front();
    if (!(working->hasChildren())) working->deny();	
    paths.pop_front();
  }
  if (best != 0) {
    paths.push_front(best);
    mostLikelyRoom = paths.front()->getRoom();
    if (paths.begin()++ == paths.end()) { // excactly one path left -> go APPROVED
      paths.front()->approve();
      paths.clear();
      state = APPROVED;
      return;
    }
  }
  else state = SYNCING;
}



Coordinate * Parser::getExpectedCoordinate() {
  Coordinate * c = cmm.activate();
  c->add(stdMoves[playerEvents.front()->type]);
  c->add(mostLikelyRoom->getCoordinate());
  return c;
}

