#ifndef MAP
#define MAP
#include "Room.h"

class Engine;
class Storage;
class Renderer;

class Map {
 public:
  Room * getRoom(int id);
  Room * getRoom(int x, int y, int z);

  /** the room contains all required info (preferred Coordinates and id) 
      The id for "unknown" is -1 
  */
  void insertRoom(Room * r);

  void deleteRoom(int id);
  void deleteRoom(int x, int y, int z);
  void deleteRoom(Room * room);

  /** marks are temporary highlightings, the renderer should place on certain rooms 
      player positions for example should be treated as marks
      I'm not quite sure if this is really the map's responsibility or rather the engines'
  */
  int createMark();
  void setMark(int markId, Room *);
  void removeMark(int markId);
  Room * getMark(int markId);
}
#endif
