#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Coordinate.h"

/**
represents a viewable Frustum in the coordinate system
 
@author alve,,,
*/
class Frustum
{
public:
  Frustum();

  ~Frustum();
  
  bool PointInFrustum(Coordinate * c);
  void NormalizePlane(int side);
  void rebuild(float * proj, float * modl);
  float getDistance(Coordinate * c);

private:
  float frustum[6][4];

};

// We create an enum of the sides so we don't have to call each side 0 or 1.
// This way it makes it more understandable and readable when dealing with frustum sides.
enum FrustumSide {
    RIGHT   = 0,        // The RIGHT side of the frustum
    LEFT    = 1,        // The LEFT  side of the frustum
    BOTTOM  = 2,        // The BOTTOM side of the frustum
    TOP     = 3,        // The TOP side of the frustum
    BACK    = 4,        // The BACK side of the frustum
    FRONT   = 5         // The FRONT side of the frustum
}; 

// Like above, instead of saying a number for the ABC and D of the plane, we
// want to be more descriptive.
enum PlaneData {
    A = 0,              // The X value of the plane's normal
    B = 1,              // The Y value of the plane's normal
    C = 2,              // The Z value of the plane's normal
    D = 3               // The distance the plane is from the origin
};


#endif
