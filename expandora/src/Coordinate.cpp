#include "Coordinate.h"

vector<Coordinate *> stdMoves;
ObjectRecycler<Coordinate> cmm;

Coordinate::Coordinate(int _x, int _y, int _z) {
	x = _x;
	y = _y;
	z = _z;
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
