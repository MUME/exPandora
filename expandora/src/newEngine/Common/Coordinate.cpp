#include "Coordinate.h"


vector<Coordinate *> Coordinate::stdMoves; //= *(new vector<Coordinate *>);
map<QString, char> Coordinate::moveCodes; //= *(new map<QString, char>);




ObjectRecycler<Coordinate> cmm;

void Coordinate::insertMoves(map<QString, Coordinate *> & moves) {
  map<QString, Coordinate *>::iterator i = moves.begin();
  char pos;
  for(; i != moves.end(); ++i) {
    QString name = i->first;
    Coordinate * value = i->second;
    if (moveCodes.find(name) == moveCodes.end()) {
      moveCodes.insert(make_pair(name, pos = stdMoves.size()));
      stdMoves.resize(pos+1);
      stdMoves[pos] = value;
    }
    else {
      pos = moveCodes.find(name)->second;
      delete (stdMoves[pos]);
      stdMoves[pos] = value; // overwriting ...
    }
  }
}

int Coordinate::distance(Coordinate * other) {
  int ret = abs(x - other->x);
  ret += abs(y - other->y);
  ret += abs(z - other->z);
  return ret;
}

void Coordinate::clear() {
  x = 0;
  y = 0;
  z = 0;
}

Coordinate * Coordinate::go(BaseEvent * ev) {
  if (ev->type < 0) return 0;
  Coordinate * ret = cmm.activate();
  ret->x = x;
  ret->y = y;
  ret->z = z;
  ret->add(stdMoves[ev->type]);	
  return ret;
}

void Coordinate::add(Coordinate * other) {
  x += other->x;
  y += other->y;
  z += other->z;
}
