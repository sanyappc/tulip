/*
	  ParallelTools.cpp

   Created on: 1 avr. 2009
       Author: Antoine Lambert
       E-mail: antoine.lambert@labri.fr

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

 */

#include "ParallelTools.h"

namespace tlp {

void rotateVector(Coord &vec, double alpha, int rot) {
  Coord backupVec(vec);
  double aRot =  2.0*M_PI * alpha / 360.0;
  float cosA = cos(aRot);
  float sinA = sin(aRot);
  switch(rot) {
  case Z_ROT:
    vec[0] = backupVec[0]*cosA - backupVec[1]*sinA;
    vec[1] = backupVec[0]*sinA + backupVec[1]*cosA;
    break;
  case Y_ROT:
    vec[0] = backupVec[0]*cosA + backupVec[2]*sinA;
    vec[2] = backupVec[2]*cosA - backupVec[0]*sinA;
    break;
  case X_ROT:
    vec[1] = backupVec[1]*cosA - backupVec[2]*sinA;
    vec[2] = backupVec[1]*sinA + backupVec[2]*cosA;
    break;
  }
}

Coord *computeStraightLineIntersection(const Coord line1[2], const Coord line2[2]) {

  Coord *intersectionPoint = NULL;
  bool line1ParallelToXaxis = false;
  bool line1ParallelToYaxis = false;
  bool line2ParallelToXaxis = false;
  bool line2ParallelToYaxis = false;
  bool parallelLines = false;
  float x, y;

  // compute the equation of the first line
  // y = al1 * x + bl1
  float xal1 = line1[0].getX();
  float xbl1 = line1[1].getX();
  float yal1 = line1[0].getY();
  float ybl1 = line1[1].getY();
  float al1 = 0;
  float bl1 = 0;
  if ((xbl1 - xal1) != 0) {
    al1 = (ybl1 - yal1) / (xbl1 - xal1);
    bl1 = ybl1 - al1 * xbl1;
  } else {
    line1ParallelToYaxis = true;
  }

  // compute the equation of the second line
  // y = al2 * x + bl2
  float xal2 = line2[0].getX();
  float xbl2 = line2[1].getX();
  float yal2 = line2[0].getY();
  float ybl2 = line2[1].getY();
  float al2 = 0;
  float bl2 = 0;
  if ((xbl2 - xal2) != 0) {
    al2 = (ybl2 - yal2) / (xbl2 - xal2);
    bl2 = ybl2 - al2 * xbl2;
  } else {
    line2ParallelToYaxis = true;
  }

  if (!line1ParallelToYaxis && al1 == 0) {
    line1ParallelToXaxis = true;
  }

  if (!line2ParallelToYaxis && al2 == 0) {
    line2ParallelToXaxis = true;
  }

  // compute the intersection point of the two lines if any
  if (line1ParallelToXaxis && line2ParallelToYaxis) {
    x = xal2;
    y = yal1;
  } else if (line2ParallelToXaxis && line1ParallelToYaxis) {
    x = xal1;
    y = yal2;
  } else if (line1ParallelToXaxis && al2 != 0) {
    y = yal1;
    x = (y - bl2) / al2;
  } else if (line2ParallelToXaxis && al1 != 0) {
    y = yal2;
    x = (y - bl1) / al1;
  } else if(line1ParallelToYaxis && !line2ParallelToYaxis) {
	  x = xal1;
	  y = al2 * x + bl2;
  } else if(line2ParallelToYaxis && !line1ParallelToYaxis) {
	  x = xal2;
	  y = al1 * x + bl1;
  } else if (al1 != al2) {
    float d1 = (bl2 - bl1);
    float d2 = (al1 - al2);
    x = d1 / d2;
    y = al1 *x + bl1;
  } else {
    parallelLines = true;
  }

  if (!parallelLines) {
    intersectionPoint = new Coord(x, y, 0);
  }

  return intersectionPoint;
}

}
