#include "Parser.h"


#include "RoomCollection.h"
#include <stack>


Parser::Parser() : insertLock(true) {
  mostLikelyRoom = 0;
  state = SYNCING;
  matchingTolerance = 0;
  pathAcceptance = 5.0;
  paths = new list<Path *>();
  shortPaths = 0;
}

void Parser::setTerrain(Property * ter) {
  activeTerrain = ter->get(0); // the first character has to be the terrain id
}

void Parser::dropNote(ParseEvent * note) {
  if (mostLikelyRoom != 0) { 
    while (note->next() != 0) mostLikelyRoom->addOptional(note->current());
  }
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
      dropNote(ev);
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
  if ((perhaps = possible->matchOne(mudEvents.front(), matchingTolerance)) != 0) { // narrows possible by the event and return a Room if exactly one is left
    emit playerMoved(mostLikelyRoom->getCoordinate(), perhaps->getCoordinate());
    mostLikelyRoom = perhaps;
  }
  else { // try to match by coordinate
    Coordinate * c = getExpectedCoordinate(mostLikelyRoom);
    inputLock.lock();
    emit lookingForRooms(c);
    inputLock.unlock();
    //perhaps = admin->getRoom(c);
    if (state == DANGLING_APPROVED) state = APPROVED;
    cmm.deactivate(c);
  }
  rcmm.deactivate(possible);
  mudPop();
  playerPop(); 
}

void Parser::approvedRoom(Room * perhaps) {  
  if ((perhaps == 0) || (!perhaps->fastCompare(mudEvents.front(), matchingTolerance))) {
    // not found - go EXPERIMENTING
    state = EXPERIMENTING;
    experimentingRoom(mostLikelyRoom);
    isertLock.unlock();
    experimenting();
    insertLock.lock();
  }
  else { // found => add the exit
    state = DANGLING_APPROVED;
    mostLikelyRoom->addExit(playerEvents.front()->type, perhaps);
    emit playerMoved(mostLikelyRoom->getCoordinate(), perhaps->getCoordinate());
    mostLikelyRoom = perhaps;
  }
}

void Parser::syncingRoom(Room * room) {
    state = DANGLING_APPROVED;
    lastMostLikely = mostLikelyRoom;
    mostLikelyRoom = room;
    emit playerMoved(0, mostLikelyRoom->getCoordinate());
}

void Parser::match(Room * room) {
  insertLock.lock();
  if (state == DANGLING_APPROVED) { // a second room was found in approved
    state = EXPERIMENTING;
    experimentingRoom(lastMostLikely);
    insertLock.unlock();
    experimenting();
    return;
  }
  else if (state == APPROVED) approvedRoom(room);
  else if (state == EXPERIMENTING) experimentingRoom(room);
  else if (state == SYNCING) syncingRoom(room);
  inserLock.unlock();
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

  insertLock.lock();
  emit lookingForRooms(this, mudEvents.front());
  insertLock.unlock();
  if (state == DANGLING_APPROVED) state = APPROVED;
  //RoomSearchNode * possible = admin->getRooms(mudEvents.front());
  if (!(playerEvents.empty())) playerPop();
  mudPop();
}	

void Parser::unify() {
  state = APPROVED;
  
  Room * old = mostLikelyRoom;
  mostLikelyRoom = paths.front()->getRoom();
  emit playerMoved(old->getCoordinate(), mostLikelyRoom->getCoordinate());
  
  paths.front()->approve();
  paths.pop_front();
  list<Path *>::iterator i = paths.begin();
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

  prevBest = paths.front()->getProb();
  shortPaths = paths;
  paths = new list<Path *>();
  best = 0;
  seconf = 0;

  insertLock.lock(); // make sure no other parser pushes rooms in our paths
  emit lookingForRooms(mudEvents.front());
  
  state = INSERTING;
  for (i = shortPaths->begin(); i != shortPaths->end(); ++i) {
    c = getExpectedCoordinate((*i)->getRoom());
    emit newSimilarRoom(mudEvents.front(), c, activeTerrain);
    cmm.deactivate(c);
  }
  state = EXPERIMENTING;
  insertLock.unlock();

  Path * working = 0;
  for (i = shortPaths.begin(); i != shortPaths.end(); ++i) {
    working = paths.front();
    paths.pop_front();
    if (!(working->hasChildren())) working->deny();	
  }
 
  if (best != 0) {
    paths.push_front(best);
    if (second == 0 || best->getProb() > second->getProb()*pathAcceptance) { // excactly one path left -> go APPROVED
      unify();
    }
    else {
      Room * old = mostLikelyRoom;
      mostLikelyRoom = paths.front()->getRoom();
      emit playerMoved(old->getCoordinate(), mostLikelyRoom->getCoordinate());
    }
  }
  else {
    state = SYNCING;
    emit playerMoved(mostLikelyRoom->getCoordinate(), 0);
    mostLikelyRoom = 0;
  }

  while (!releaseSchedule.empty()) {
    releaseSchedule.top()->release();
    releaseSchedule.pop();
  }

  delete(shortPaths);
  shortPaths = 0;
  playerPop();
  mudPop();
}

void Parser::experimentingRoom(Room * room) {
  if (shortPaths == 0) {
    paths->push_front(pamm.activate());
    paths->front()->init(room);
  }
  else {
    for (i = shortPaths.begin(); i != shortPaths.end(); ++i) {
      Coordinate * c = getExpectedCoordinate((*i)->getRoom());
      Path * working = (*i)->fork(room, c);
      checkPath(working);
      cmm.deactivate(c);
    }
  }
}


void Parser::newRoom(Room * newRoom) {
  insertLock.lock();
  if (state == INSERTING) {
    newRoom->hold();
    releaseSchedule.push(newRoom);
    Path * working = (*i)->fork(newRoom, c);
    working->setProb(working->getProb()/pathAcceptance);
    checkPath(working);
  }
  insertLock.unlock();
}

void Parser::checkPath(Path * working) {
  if (working->getProb() < prevBest/pathAcceptance) {
    (*i)->removeChild(working);
    pamm.deactivate(working);
  }
  else {
    if (best == 0) best = working;
    else if (working->getProb() > best->getProb()) {
      paths.push_back(best);
      second = best;
      best = working;
    }
    else {
      if (second == 0) second = working;
      paths.push_back(working);
   }
  }
}






Coordinate * Parser::getExpectedCoordinate(Room * base) {
  Coordinate * c = cmm.activate();
  if (!playerEvents.empty()) c->add(Coordinate::stdMoves[playerEvents.front()->type]);
  if (base != 0) c->add(base->getCoordinate());
  return c;
}




