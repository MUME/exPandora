#include "Coordinate.h"
using namespace std;

vector<Coordinate> Coordinate::stdMoves; 
map<QString, uint> Coordinate::moveCodes; 


void Coordinate::insertMoves(map<QString, Coordinate> & moves) {
  map<QString, Coordinate>::iterator i = moves.begin();
  uint pos;
  for(; i != moves.end(); ++i) {
    QString name = i->first;
    Coordinate value = i->second;
    if (moveCodes.find(name) == moveCodes.end()) {
      moveCodes[name] = (pos = stdMoves.size());
      stdMoves.resize(pos+1);
      stdMoves[pos] = value;
    }
    else {
      pos = moveCodes[name];
      stdMoves[pos] = value; // overwriting ...
    }
  }
}

bool Coordinate::operator== (Coordinate & other) {
  return (other.x == x && other.y == y && other.z == z);
}

bool Coordinate::operator!= (Coordinate & other) {
  return (other.x != x || other.y != y || other.z != z);
}

int Coordinate::distance(Coordinate & other) {
  int ret = abs(x - other.x);
  ret += abs(y - other.y);
  ret += abs(z - other.z);
  return ret;
}

void Coordinate::clear() {
  x = 0;
  y = 0;
  z = 0;
}

Coordinate Coordinate::go(BaseEvent * ev) {
  Coordinate ret;
  if (ev->type != MOVE) return ret;
  ret += *this;
  ret += stdMoves[ev->subType];	
  return ret;
}

void Coordinate::operator+=(Coordinate & other) {
  x += other.x;
  y += other.y;
  z += other.z;
}

void Coordinate::operator-=(Coordinate & other) {
  x -= other.x;
  y -= other.y;
  z -= other.z;
}

Coordinate Coordinate::operator+(Coordinate & other) {
  Coordinate ret;
  ret += *this;
  ret += other;
  return ret;
}

Coordinate Coordinate::operator-(Coordinate & other) {
  Coordinate ret;
  ret += *this;
  ret -= other;
  return ret;
}
