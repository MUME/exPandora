#ifndef ENGINE
#define ENGINE

class Proxy;
class Map;
class Room;

class Engine {
 public:
  void attachProxy(Proxy * p);
  void attachMap(Map * m);

  /* include a fully specified new room in the search structure */  
  void roomNew(Room * r);

  /* an existing room changed, reflect the changes in the search structure */
  void roomChanged(Room * r);

  /* this room will get deleted after all engines have been notified about it */  
  void roomDeletion(Room * r);

  /* analyze a stream from the mud or the player. These methods are supposed to return
     quickly, deciding if the text should be passed on, meaning that the next engine
     analyzes it or it is sent if this is the last engine. The buffer stays in the calling
     method's responsibility, so it has to be copied if the engine wants to do further analyzing
     on it after returning from these methods.
  */
  boolean analyzeMudStream(char * buffer, int length);
  boolean analyzeUserStream(char * buffer, int length);
}
#endif
