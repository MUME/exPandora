#ifndef ROOM
#define ROOM
#include "Terrain.h"

/**
   "pointer lists" like char ** are 0-terminated.
   all Room implemenations should be able to construct themselves on top of this interface.
   it should be possible to have several engine-internal implementations on the same Room. 
   all get-methods return 0 if the respective thing doesn't exist

   => we have to provide a "basic" and an "adapter" implementation for each different room implementation
   this means a lot of trouble for different room implementations - perhaps we can find an easier solution
   
   another problem is different semantics for exits: the new engine treats exits with doors as optional
   because they might be secret, the old engine treats all exits as required even if they are secret
   I don't have a good solution for this problem yet
*/

class Room {
 public:
  virtual void setTerrain(Terrain * t);

  /** the problem with this method is that it will have different semantics for old and new code
      the new code will add the destination the old one will replace it */
  virtual void addNeighbour(int direction, Room * destination);
  virtual void setRequired(char * text, int pos);
  virtual void addRequired(char * property);
  virtual Room ** getNeighbours(int direction);
  virtual void setOptional(char * text, int pos);
  virtual void addOptional(char * note);
  virtual void setId(int id); 
  virtual int getId();

  /** finds out if the given strings are contained in the rooms optional properties */
  virtual bool containsOptionals(char ** optionals);

  /** finds out if the given strings match the room's required properties */
  virtual bool matchesRequired(char ** props, int tolerance);

  /** returns the pos-th property - 0 would be room name, 1 desc, and so on */
  virtual char * getRequired(int pos);

  /** returns the pos-th optional property */
  virtual char * getOptional(int pos);
  

  virtual void setCoordinate(int x, int y, int z); 
  virtual int getX();
  virtual int getY();
  virtual int getZ();

  /** storing timestamps in literal form is really ugly - this needs to be changed in the old code */
  /* we should store the time as the number of msecs since  1970 - that's standard */
  virtual long getTime();
  virtual void setTime(long ts) ;
  
  /** return some Id of the implementation so that an engine knows if it has to attach an adapter for 
      there should really be a cleaner way of doing this in c++ ... perhaps I missed something */
  virtual int getImplementation();
}
