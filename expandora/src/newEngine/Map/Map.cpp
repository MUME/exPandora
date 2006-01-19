#include "Map.h"
#include <stdexcept>
#include <vector>

/**
 * doesn't modify c
 */
bool Map::defined(Coordinate & in_c)
{
  Coordinate c = in_c;
  vector<vector<vector<Room *> > > & segment = getSegment(c);

  if (segment.size() > (uint)c.x &&
      segment[c.x].size() > (uint)c.y &&
      segment[c.x][c.y].size() > (uint)c.z &&
      segment[c.x][c.y][c.z])
  {
    
    return true;
  }
  else
  {
    
    return false;
  }

}


Room * Map::get(Coordinate &c)
{
  if (!defined(c)) return 0;
  else
  {
    Coordinate cm = c;
    vector<vector<vector<Room *> > > & segment = getSegment(cm);
    Room * ret = segment[cm.x][cm.y][cm.z];
    return ret;
  }
}

/**
 * modifies c so that it fits the found segment
 */
vector<vector<vector<Room *> > > & Map::getSegment(Coordinate & c)
{
  int xs, ys, zs;
  if(c.x < 0) xs = 0, c.x*=-1;
  else xs = 1;
  if(c.y < 0) ys = 0, c.y*=-1;
  else ys = 1;
  if(c.z < 0) zs = 0, c.z*=-1;
  else zs = 1;
  return map[xs][ys][zs];
}



/**
 * doesn't modify c
 */
void Map::set(Coordinate &cm, Room *room)
{
  Coordinate c = cm;
  vector<vector<vector<Room *> > > & segment = getSegment(c);
  if (segment.size() <= (uint)c.x) segment.resize((uint)c.x + 1);
  if (segment[(uint)c.x].size() <= (uint)c.y) segment[(uint)c.x].resize((uint)c.y + 1);
  if (segment[(uint)c.x][(uint)c.y].size() <= (uint)c.z) segment[(uint)c.x][(uint)c.y].resize((uint)c.z + 1, 0);
  segment[c.x][c.y][c.z] = room;
}

/**
 * gets a new coordinate but doesn't return the old one ... should probably be changed ...
 */
Coordinate Map::setNearest(Coordinate & in_c, Room *room)
{
  Coordinate c = getNearestFree(in_c);
  set(c, room);
  room->setCoordinate(c);
  return c;
}

Coordinate Map::getNearestFree(Coordinate & p)
{
  Coordinate c;
  int distance = 1;
  int x  = 0, y = 0, z = 0;
  while(1)
  {
    for (x = 0; x < distance; x++)
    {
      for (y = 0; x+y < distance; y++)
      {
        for (z = 0; x+y+z < distance; z++)
        {
          c.x=p.x+x, c.y=p.y+y, c.z=p.z+z;
          if (!defined(c)) return c;
          c.z=p.z-z;
          if (!defined(c)) return c;
          c.y=p.y-y, c.z=p.z+z;
          if (!defined(c)) return c;
          c.z=p.z-z;
          if (!defined(c)) return c;
          c.x=p.x-x, c.y=p.y+y;c.z=p.z+z;
          if (!defined(c)) return c;
          c.z=p.z-z;
          if (!defined(c)) return c;
          c.y=p.y-y, c.z=p.z+z;
          if (!defined(c)) return c;
          c.z=p.z-z;
          if (!defined(c)) return c;
        }
      }
    }
    distance++;
  }
  return 0;
}
