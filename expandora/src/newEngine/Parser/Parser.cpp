#include "Parser.h"


#include "RoomCollection.h"
#include <stack>


/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
extern "C" MY_EXPORT Component * createComponent()
{
  return new Parser;
}



Parser::Parser() : Component(true) {
	
  mostLikelyRoom = 0;
  state = SYNCING;
  matchingTolerance = 0;
  remoteMapDelay = 0;
  pathAcceptance = 5.0;
  paths = new list<Path *>;
}





void Parser::setTerrain(Property * ter)
{
  activeTerrain = ter->get(0); // the first character has to be the terrain id
}

void Parser::dropNote(ParseEvent * note)
{
  if (mostLikelyRoom != 0)
  {
    while (note->next() != 0) mostLikelyRoom->addOptional(note->current());
  }
  pemm.deactivate(note);
}

void Parser::event(ParseEvent * ev)
{
  //parserMutex.lock();
  if (ev->type >= 0)
  {
    // a move event
    playerEvents.push(ev);
  }

  else
  {

    switch (ev->type)
    {
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
  //parserMutex.unlock();
  //parserSync.wakeOne();
  checkQueues();
}

void Parser::checkQueues()
{
  if (mudEvents.empty()) return;
  if (playerEvents.empty())
  {
    if (state != SYNCING) state = SYNCING;
  }
  else while ( (!(mudEvents.empty())) && mudEvents.front()->timestamp < playerEvents.front()->timestamp)
    {
      mudPop();
      state = SYNCING;
    }
  if (mudEvents.empty()) return;

  //now we are sure we have a user event that happened before the mud event


  switch (state)
  {
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


void Parser::approved()
{
  if (playerEvents.front()->type == UNIQUE)
  {
    mostLikelyRoom->setUnique();
    playerPop();
    return;
  }
  if (mudEvents.front()->type == MOVE_FAIL)
  {
    mudPop();
    playerPop();
    return;
  }


  // now we have a move and a room on the event queues;

  Approved appr(mudEvents.front(), matchingTolerance);
  set<int> * possible = mostLikelyRoom->go(playerEvents.front());
  for (set<int>::iterator i = possible->begin(); i != possible->end(); ++i)
  {
    emit lookingForRooms(&appr, *i);
  }

  //QObject::thread()->wait(remoteMapDelay);

  Room * perhaps = appr.oneMatch();

  Coordinate * c = 0;
  if (perhaps == 0)
  { // try to match by coordinate
    appr.reset();
    c = getExpectedCoordinate(mostLikelyRoom);
    emit lookingForRooms(&appr, c); //needs to be synchronous
    //QThread::usleep(remoteMapDelay);
    perhaps = appr.oneMatch();
    if (perhaps != 0)
    {
      //parserMutex.lock();
      QObject::connect(this, SIGNAL(addExit(int, int, int)), appr.getOwner(), SLOT(addExit(int, int, int)), Qt::DirectConnection);
      emit addExit(mostLikelyRoom->getId(), perhaps->getId(), playerEvents.front()->type);
      QObject::disconnect(this, SIGNAL(addExit(int, int, int)), 0,0);
      //parserMutex.unlock();
    }
    cmm.deactivate(c);
  }
  if (perhaps != 0)
  {
    emit playerMoved(mostLikelyRoom->getCoordinate(), perhaps->getCoordinate());
    mostLikelyRoom = perhaps;
  }
  else
  {
    state = EXPERIMENTING;
    Path * root = pamm.activate();
    root->init(mostLikelyRoom, 0);
    paths->push_front(root);
    experimenting();
  }

  

  mudPop();
  playerPop();
}

/*
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
*/

void Parser::playerPop()
{
  pemm.deactivate(playerEvents.front());
  playerEvents.pop();
}

void Parser::mudPop()
{
  pemm.deactivate(mudEvents.front());
  mudEvents.pop();
}

void Parser::syncing()
{
  if (mudEvents.empty()) return;
  if (!(playerEvents.empty()) && (playerEvents.front()->type == UNIQUE))
  {
    // unique doesn't work when syncing ...
    playerPop();
    return;
  }
  if (mudEvents.front()->type == MOVE_FAIL)
  {
    mudPop();
    if (!playerEvents.empty()) playerPop();
    return;
  }

  // now we have a move and a room on the event queues;

  Syncing sync(paths);
  emit lookingForRooms(&sync, mudEvents.front());
  //QThread::usleep(remoteMapDelay);

  paths = sync.evaluate();
  evaluatePaths();

  if (!(playerEvents.empty())) playerPop();
  mudPop();
}


void Parser::experimenting()
{
  if (playerEvents.front()->type == UNIQUE)
  {
    // do something ....
    playerPop();
    return;
  }
  if (mudEvents.front()->type == MOVE_FAIL)
  {
    mudPop();
    playerPop();
    return;
  }

  Experimenting exp(this, paths, pathAcceptance);

  for (list<Path *>::iterator i = paths->begin(); i != paths->end(); ++i)
  {
    Coordinate * c = getExpectedCoordinate((*i)->getRoom());
    emit createRoom(mudEvents.front(), c, activeTerrain);
    cmm.deactivate(c);
  }

  emit lookingForRooms(&exp, mudEvents.front());

  //QThread::usleep(remoteMapDelay);

  paths = exp.evaluate();
  

  evaluatePaths();
  playerPop();
  mudPop();
}

void Parser::evaluatePaths()
{
  Coordinate * oldCoord = 0;
  if (mostLikelyRoom)
    oldCoord = mostLikelyRoom->getCoordinate();
  Coordinate * newCoord = 0;
  if (paths->empty())
  {
    state = SYNCING;
    mostLikelyRoom = 0;
  }
  else if (++paths->begin() == paths->end())
  {
    state = APPROVED;
    mostLikelyRoom = paths->front()->getRoom();
    newCoord = mostLikelyRoom->getCoordinate();
    paths->front()->approve();
    paths->pop_front();
  }
  else
  {
    mostLikelyRoom = paths->front()->getRoom();
    newCoord = mostLikelyRoom->getCoordinate();
  }

  if (newCoord != oldCoord) emit playerMoved(oldCoord, newCoord);

}



Coordinate * Parser::getExpectedCoordinate(Room * base)
{
  Coordinate * c = cmm.activate();
  if (!playerEvents.empty()) c->add(Coordinate::stdMoves[playerEvents.front()->type]);
  if (base != 0) c->add(base->getCoordinate());
  return c;
}




