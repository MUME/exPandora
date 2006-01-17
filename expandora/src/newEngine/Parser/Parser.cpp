
#include <stack>

#include "Parser.h"




/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
#ifndef MONOLITHIC
extern "C" MY_EXPORT Component * createComponent()
{
  return new Parser;
}
#else
Initializer<Parser> parser("Parser");
#endif

using namespace Qt;

Parser::Parser() : 
  Component(true), 
  signaler(this),
  mostLikelyRoom(0),
  state(SYNCING),
  matchingTolerance(0),
  paths(new list<Path *>)
{}

void Parser::init() {
  QObject::connect(&signaler, SIGNAL(addExit(int, int, int)), this, SIGNAL(addExit(int, int, int)));
}

/**
 * the signals - apart from playerMoved have to be DirectConnections because we want to 
 * be sure to find all available rooms and we also want to make sure a room is actually
 * inserted into the map before we search for it
 * The slots need to be queued because we want to make sure all data is only accessed
 * from this thread
 */
ConnectionType Parser::requiredConnectionType(const QString & str) {
  
  if (str == SLOT(event(ParseEvent *)) || str == SLOT(setTerrain(Property *)))
    return QueuedConnection;
  else if (str == SIGNAL(playerMoved(Coordinate *, Coordinate *)))
    return AutoCompatConnection;
  else 
    return DirectConnection;
}

// this slot will eventually be replaced by looking into the hints of each property
void Parser::setTerrain(Property * ter)
{
  activeTerrain = ter->get(0); // the first character has to be the terrain id
}

void Parser::dropNote(ParseEvent * note)
{
  if (mostLikelyRoom)
  {
    while (note->next()) mostLikelyRoom->addOptional(note->current());
  }
  pemm.deactivate(note);
}

void Parser::event(ParseEvent * ev)
{
  
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
  
  checkQueues();
}

void Parser::checkQueues()
{
  if (mudEvents.empty()) return;
  if (playerEvents.empty())
  {
    if (state != SYNCING) state = SYNCING;
  }
  else while ( !mudEvents.empty() && mudEvents.front()->timestamp < playerEvents.front()->timestamp)
    {
      mudPop();
      state = SYNCING;
    }
  if (mudEvents.empty()) return;

  // now we are sure we have a user event that happened before the mud event
  // or we only have a mud event and are in syncing state

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

  Approved appr(this, mudEvents.front(), matchingTolerance);
  set<int> * possible = mostLikelyRoom->go(playerEvents.front());
  for (set<int>::iterator i = possible->begin(); i != possible->end(); ++i)
  {
    emit lookingForRooms(&appr, *i);
  }

  Room * perhaps = appr.oneMatch();

  Coordinate * c = 0;
  if (perhaps == 0)
  { // try to match by coordinate
    appr.reset();
    c = getExpectedCoordinate(mostLikelyRoom);
    emit lookingForRooms(&appr, c); //needs to be synchronous
    
    perhaps = appr.oneMatch();
    if (perhaps)
    {

      emit addExit(mostLikelyRoom->getId(), perhaps->getId(), playerEvents.front()->type);

    }
    cmm.deactivate(c);
  }
  if (perhaps)
  {
    emit playerMoved(mostLikelyRoom->getCoordinate(), perhaps->getCoordinate());
    mostLikelyRoom = perhaps;
    mudPop();
    playerPop();
  }
  else
  {
    state = EXPERIMENTING;
    Path * root = pamm.activate();
    root->init(mostLikelyRoom, 0, 0, &signaler);
    paths->push_front(root);
    experimenting();
  }
}



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

  Syncing sync(paths, &signaler);
  emit lookingForRooms(&sync, mudEvents.front());

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

  Experimenting exp(this, paths, playerEvents.front()->type);

  for (list<Path *>::iterator i = paths->begin(); i != paths->end(); ++i)
  {
    Coordinate * c = getExpectedCoordinate((*i)->getRoom());
    emit createRoom(mudEvents.front(), c, activeTerrain);
    cmm.deactivate(c);
  }

  emit lookingForRooms(&exp, mudEvents.front());

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




