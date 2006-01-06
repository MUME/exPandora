#include "Frustum.h"
#include <cmath>

Frustum::Frustum()
{}


Frustum::~Frustum()
{}




void Frustum::NormalizePlane(int side)
{
  // Here we calculate the magnitude of the normal to the plane (point A B C)
  // Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
  // To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
  float magnitude = (float)sqrt( frustum[side][A] * frustum[side][A] +
                                 frustum[side][B] * frustum[side][B] +
                                 frustum[side][C] * frustum[side][C] );

  // Then we divide the plane's values by it's magnitude.
  // This makes it easier to work with.
  frustum[side][A] /= magnitude;
  frustum[side][B] /= magnitude;
  frustum[side][C] /= magnitude;
  frustum[side][D] /= magnitude;
}

bool Frustum::PointInFrustum(Coordinate * c)
{
  // Go through all the sides of the frustum
  for(int i = 0; i < 6; ++i )
  {
    // Calculate the plane equation and check if the point is behind a side of the frustum
    if(frustum[i][A] * c->x + frustum[i][B] * c->y + frustum[i][C] * c->z + frustum[i][D] <= 0)
    {
      // The point was behind a side, so it ISN'T in the frustum
      return false;
    }
  }

  // The point was inside of the frustum (In front of ALL the sides of the frustum)
  return true;
}



void Frustum::rebuild(float * proj, float * modl)
{
  float   clip[16];                               // This will hold the clipping planes

  // To combine 2 matrices, we multiply them.
  clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
  clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
  clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
  clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

  clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
  clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
  clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
  clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

  clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
  clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
  clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
  clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

  clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
  clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
  clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
  clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

  // Now we actually want to get the sides of the frustum.  To do this we take
  // the clipping planes we received above and extract the sides from them.

  // This will extract the RIGHT side of the frustum
  frustum[RIGHT][A] = clip[ 3] - clip[ 0];
  frustum[RIGHT][B] = clip[ 7] - clip[ 4];
  frustum[RIGHT][C] = clip[11] - clip[ 8];
  frustum[RIGHT][D] = clip[15] - clip[12];

  // Now that we have a normal (A,B,C) and a distance (D) to the plane,
  // we want to normalize that normal and distance.

  // Normalize the RIGHT side
  NormalizePlane(RIGHT);

  // This will extract the LEFT side of the frustum
  frustum[LEFT][A] = clip[ 3] + clip[ 0];
  frustum[LEFT][B] = clip[ 7] + clip[ 4];
  frustum[LEFT][C] = clip[11] + clip[ 8];
  frustum[LEFT][D] = clip[15] + clip[12];

  // Normalize the LEFT side
  NormalizePlane(LEFT);

  // This will extract the BOTTOM side of the frustum
  frustum[BOTTOM][A] = clip[ 3] + clip[ 1];
  frustum[BOTTOM][B] = clip[ 7] + clip[ 5];
  frustum[BOTTOM][C] = clip[11] + clip[ 9];
  frustum[BOTTOM][D] = clip[15] + clip[13];

  // Normalize the BOTTOM side
  NormalizePlane(BOTTOM);

  // This will extract the TOP side of the frustum
  frustum[TOP][A] = clip[ 3] - clip[ 1];
  frustum[TOP][B] = clip[ 7] - clip[ 5];
  frustum[TOP][C] = clip[11] - clip[ 9];
  frustum[TOP][D] = clip[15] - clip[13];

  // Normalize the TOP side
  NormalizePlane(TOP);

  // This will extract the BACK side of the frustum
  frustum[BACK][A] = clip[ 3] - clip[ 2];
  frustum[BACK][B] = clip[ 7] - clip[ 6];
  frustum[BACK][C] = clip[11] - clip[10];
  frustum[BACK][D] = clip[15] - clip[14];

  // Normalize the BACK side
  NormalizePlane(BACK);

  // This will extract the FRONT side of the frustum
  frustum[FRONT][A] = clip[ 3] + clip[ 2];
  frustum[FRONT][B] = clip[ 7] + clip[ 6];
  frustum[FRONT][C] = clip[11] + clip[10];
  frustum[FRONT][D] = clip[15] + clip[14];

  // Normalize the FRONT side
  NormalizePlane(FRONT);
/*
  // find out the center of the frustum
  // first get the origin of the coordinate system
  // get the distance of the origin to the center
  // the distance to front is negative, the one to back is positive
  float dist = (frustum[BACK][A] * clip[12]
               +	frustum[BACK][B] * clip[13]
               +	frustum[BACK][C] * clip[14]
               +	frustum[BACK][D]
               -	frustum[FRONT][A] * clip[12]
               -	frustum[FRONT][B] * clip[13]
               -	frustum[FRONT][C] * clip[14]
               -	frustum[FRONT][D]) / 2.0 ;



  // translate the origin by dist*(normal vector of FRONT)
  center.x = (int)(clip[12] + frustum[FRONT][A]*dist);
  center.y = (int)(clip[13] + frustum[FRONT][B]*dist);
  center.z = (int)(clip[14] + frustum[FRONT][C]*dist);
*/
}


float Frustum::getDistance(Coordinate * c, int side)
{
  return frustum[side][A] * c->x + frustum[side][B] * c->y + frustum[side][C] * c->z + frustum[side][D];
}
