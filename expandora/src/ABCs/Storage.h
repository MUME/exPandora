#ifndef STORAGE
#define STORAGE

class Map;

class Storage {
 public:
  void attachMap(Map * m);

  /** read all rooms from the specified storage and push them into each of the maps */
  void readStorage(char ** argv, int argc);

  /** write all the rooms from all maps into the specified storage */
  void writeStorage(char ** argv, int argc);
}
#endif
