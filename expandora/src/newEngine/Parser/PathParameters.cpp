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
  acceptBestRelative(10.0),
  acceptBestAbsolute(3.0),
  newRoomPenalty(5.0),
  correctPositionBonus(5.1),
  maxPaths(500.0)
{
}


PathParameters::~PathParameters()
{
}


