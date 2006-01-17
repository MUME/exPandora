//
// C++ Interface: PathParameters
//
// Description: 
//
//
// Author: alve,,, <alve@ente>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PATHPARAMETERS_H
#define PATHPARAMETERS_H

/**
@author alve,,,
*/
class PathParameters{
public:
    PathParameters();
    double acceptBestRelative;
    double acceptBestAbsolute;
    double newRoomPenalty;
    double correctPositionBonus;
    double maxPaths;
    
    ~PathParameters();

};

#endif
