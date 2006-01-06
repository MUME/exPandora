#include "Terrain.h"

vector<Terrain *> Terrain::terrains = *(new vector<Terrain *>);
map<QString, char> Terrain::terrainIDs = *(new map<QString, char>);
