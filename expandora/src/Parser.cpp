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
  if ((perhaps = possible->matchOne(mudEvents.front())) != 0) { // narrows possible by the event and return a Room if exactly one is left
    mostLikelyRoom = perhaps;
    rcmm.deactivate(possible);
    mudPop();
    playerPop();
    return;
  }
  else { // try to match by coordinate
    Coordinate * c = getExpectedCoordinate(mostLikelyRoom);
    perhaps = admin->getRoom(c);
    if ((perhaps == 0) || (!perhaps->fastCompare(mudEvents.front()))) {
      // not found - go EXPERIMENTING
      state = EXPERIMENTING;
      RoomSearchNode * pathStart = admin->getRooms(mudEvents.front());
      if (pathStart->numRooms() == -1) pathStart = rcmm.activate();
      buildPaths((RoomCollection *)pathStart);
      rcmm.deactivate((RoomCollection *)pathStart);
      rcmm.deactivate(possible);
      mudPop();
      playerPop();
      return;
    }
    else { // found => add the exit
      mostLikelyRoom->addExit(playerEvents.front()->type, perhaps);
      mostLikelyRoom = perhaps;
      playerPop();
      mudPop();
      cmm.deactivate(c);
      rcmm.deactivate(possible);
      return;
    }
  }
 
 
}

void Parser::playerPop() {
  pemm.deactivate(playerEvents.front());
  playerEvents.pop();
}
			
void Parser::mudPop() {
  mudEvents.front()->recycleProperties();
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
	
  RoomSearchNode * possible = admin->getRooms(mudEvents.front());
  if (possible->numRooms() == 1) {
    state = APPROVED;
    mostLikelyRoom = *(((RoomCollection *)possible)->begin());
    rcmm.deactivate((RoomCollection *)possible);
  }
  else if (possible->numRooms() >= 0)
    rcmm.deactivate((RoomCollection *)possible);

  if (!(playerEvents.empty())) playerPop();
  mudPop();

}	

void Parser::unify() {
  state = APPROVED;
  mostLikelyRoom = paths.front()->getRoom();
  paths.front()->approve();
  paths.pop_front();
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
	
  RoomSearchNode * possible = admin->getRooms(mudEvents.front());
  if (possible->numRooms() == -1) possible = rcmm.activate();
  enlargePaths((RoomCollection *)possible);
  rcmm.deactivate((RoomCollection *) possible);
  playerPop();
  mudPop();
}

void Parser::buildPaths(RoomCollection * rc) {
  Path * working = pamm.activate();
  working->init(mostLikelyRoom, admin);
  paths.push_front(working);
  enlargePaths(rc);
}



void Parser::enlargePaths(RoomCollection * rc) {

  list<Path *>::iterator i = paths.begin();
  set<Room *>::iterator j = rc->begin();
  
  Coordinate * c = 0;
  

  Path * working = 0;
  Path * best = 0;
  Path * second = 0;
  double prevBest = paths.front()->getProb();

  int k = paths.size();
  ParseEvent * mudFront = mudEvents.front()->copy();
  

  for (int l = 0; l < k; l++) {
    c = getExpectedCoordinate((*i)->getRoom());
    working = 0;

    do {  
      if (working == 0) {
	working = (*i)->fork(admin->quickInsert(mudFront, c, activeTerrain), c);
	if (l < k-1) mudFront = mudFront->copy(); // still paths left
	else { // no paths left - no more copies needed
	  pemm.deactivate(mudFront);
	  mudFront = 0; 
	}
	working->setProb(working->getProb()/PATH_ACCEPT);
      }
      else {
	working = (*i)->fork(*j, c);
	j++;
      }

      if (working->getProb() < prevBest/PATH_ACCEPT) {
	(*i)->removeChild(working);
	pamm.deactivate(working);
      }
      else {
	if (best == 0) best = working;
	else if(working->getProb() > best->getProb()) {
	  paths.push_back(best);
	  second = best;
	  best = working;
	}
	else {
	  if (second == 0) second = working;
	  paths.push_back(working);
	}
      }
    } while (j != rc->end());

    j = rc->begin();
    i++;
    cmm.deactivate(c);
  } 



  for (int l = 0; l < k; l++) {
    working = paths.front();
    paths.pop_front();
    if (!(working->hasChildren())) working->deny();	
  }

  if (best != 0) {
    paths.push_front(best);
    mostLikelyRoom = paths.front()->getRoom();
    if (second == 0 || best->getProb() > second->getProb()*PATH_ACCEPT) { // excactly one path left -> go APPROVED
      unify();
    }
  }
  else state = SYNCING;
}


Coordinate * Parser::getExpectedCoordinate(Room * base) {
  Coordinate * c = cmm.activate();
  if (!playerEvents.empty()) c->add(stdMoves[playerEvents.front()->type]);
  if (base != 0) c->add(base->getCoordinate());
  return c;
}




