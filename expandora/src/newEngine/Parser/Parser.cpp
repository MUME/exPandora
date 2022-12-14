
#include <stack>

#include "Parser.h"

using namespace std;


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

void Parser::init()
{
  QObject::connect(&signaler, SIGNAL(addExit(int, int, uint)), this, SIGNAL(addExit(int, int, uint)));
}

/**
 * the signals - apart from playerMoved have to be DirectConnections because we want to 
 * be sure to find all available rooms and we also want to make sure a room is actually
 * inserted into the map before we search for it
 * The slots need to be queued because we want to make sure all data is only accessed
 * from this thread
 */
ConnectionType Parser::requiredConnectionType(const QString & str)
{

  if (str == SLOT(event(ParseEvent *)) || 
      str == SLOT(setTerrain(Property *)) ||
      str == SLOT(deleteMostLikelyRoom()))
    return QueuedConnection;
  else if (str == SIGNAL(playerMoved(Coordinate, Coordinate)))
    return AutoCompatConnection;
  else
    return DirectConnection;
}

// this slot will eventually be replaced by looking into the hints of each property
void Parser::setTerrain(Property * ter)
{
  activeTerrain = ter->get(0); // the first character has to be the terrain id
}


void Parser::event(ParseEvent * ev)
{

  if (ev->type == MOVE)
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

void Parser::deleteMostLikelyRoom()
{

  if (state == EXPERIMENTING)
  {
    list<Path *> * newPaths = new list<Path *>;
    Path * best = 0;
    for (list<Path*>::iterator i = paths->begin(); i != paths->end(); ++i)
    {
      Path * working = *i;
      if ((working)->getRoom() == mostLikelyRoom) working->deny();
      else if (best == 0) best = working;
      else if (working->getProb() > best->getProb())
      {
        newPaths->push_back(best);
        best = working;
      }
      else newPaths->push_back(working);
    }
    if (best) newPaths->push_front(best);
    delete paths;
    paths = newPaths;
    
  }
  else paths->clear(); // throw the parser into syncing
  
  evaluatePaths();
}

  void Parser::approved()
  {
    
    if (mudEvents.front()->type == MOVE_FAIL)
    {
      mudPop();
      playerPop();
      return;
    }


    // now we have a move and a room on the event queues;

    Approved appr(this, mudEvents.front(), matchingTolerance);
    set<int> possible = mostLikelyRoom->getNeighbours(playerEvents.front());
    for (set<int>::iterator i = possible.begin(); i != possible.end(); ++i)
    {
      emit lookingForRooms(&appr, *i);
    }

    Room * perhaps = appr.oneMatch();

    Coordinate c;
    if (perhaps == 0)
    { // try to match by coordinate
      appr.reset();
      c = getExpectedCoordinate(mostLikelyRoom);
      emit lookingForRooms(&appr, c); //needs to be synchronous

      perhaps = appr.oneMatch();
      if (perhaps)
      {

        emit addExit(mostLikelyRoom->getId(), perhaps->getId(), playerEvents.front()->subType);

      }
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

    Experimenting exp(this, paths, playerEvents.front()->subType);

    for (list<Path *>::iterator i = paths->begin(); i != paths->end(); ++i)
    {
      emit createRoom(mudEvents.front(), getExpectedCoordinate((*i)->getRoom()), activeTerrain);
    }

    emit lookingForRooms(&exp, mudEvents.front());

    paths = exp.evaluate();


    evaluatePaths();
    playerPop();
    mudPop();
  }

  void Parser::evaluatePaths()
  {
    Coordinate oldCoord;
    if (mostLikelyRoom)
      oldCoord = mostLikelyRoom->getCoordinate();
    Coordinate newCoord;
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
      state = EXPERIMENTING;
      mostLikelyRoom = paths->front()->getRoom();
      newCoord = mostLikelyRoom->getCoordinate();
    }

    if (newCoord != oldCoord) emit playerMoved(oldCoord, newCoord);

  }



  Coordinate Parser::getExpectedCoordinate(Room * base)
  {
    Coordinate c;
    if (!playerEvents.empty()) c = Coordinate::stdMoves[playerEvents.front()->subType];
    Coordinate prev = base->getCoordinate();
    if (base != 0) c += prev;
    return c;
  }




