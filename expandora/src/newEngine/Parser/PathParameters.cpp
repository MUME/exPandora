//
// C++ Implementation: PathParameters
//
// Description: 
//
//
// Author: alve,,, <alve@ente>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "PathParameters.h"

PathParameters::PathParameters() :
  acceptBest(3.0),
  throwAway(5.0),
  newRoomPenalty(4.0),
  correctPositionBonus(2.0),
  maxPaths(100.0)
{
}


PathParameters::~PathParameters()
{
}


