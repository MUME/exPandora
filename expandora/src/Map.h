#ifndef ROOMS_H 
#define ROOMS_H 

#include "SceneManager.h"

#include <vector>
#include "defines.h"
#include "CRoom.h"
#include "Regions.h"
using namespace std;


#define MAX_ROOMS       30000		/* maximal amount of rooms */

class roommanager {

    QList<CRegion *>    regions;
public:
  vector<CRoom *> rooms;   		/* rooms */
  CRoom *ids[MAX_ROOMS];	/* array of pointers */

  
  unsigned int size()  { return rooms.size(); }
  unsigned int next_free; 	/* next free id */

  unsigned int oneway_room_id;

  /* plane support */  
  roommanager();
  void init();
  void reinit();			/* reinitializer/utilizer */
  
  void addRoomNonsorted(CRoom *room);   /* use only at loading */
  void addRoom(CRoom *room);
  
  
  CRoom * getRoom(unsigned int id); /* get room by id */
  QByteArray getName(unsigned int id);
  
  
    
  int tryMergeRooms(CRoom *room, CRoom *copy, int j);
    


  void fixFreeRooms();
  CRegion *getRegionByName(QByteArray name);
  bool addRegion(QByteArray name);
  void addRegion(CRegion *reg);
  
  void sendRegionsList();  
  
  QList<CRegion *> getAllRegions();
  
  
  void deleteRoom(CRoom *r, int mode);  /* user interface function */
  void smallDeleteRoom(CRoom *r);  /* user interface function */
};

extern class roommanager Map;/* room manager */

#endif

