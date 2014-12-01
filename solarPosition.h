//
//  solarPosition.h
//
//
//  Created by Sawyer Bowman on 12/1/14.
//
//

#ifndef _solarPosition_h
#define _solarPosition_h

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <float.h>

//Calculate the angle between sun path and horizontal surface
double calcSunAngle(double dayNum, double localTime, double localLat);

//Calculate day angle [rad]
double calcDayAngle(double j);

//Calculate sun declination [rad]
double calcSunDeclination(double dayNum);

//Calculate hour angle [rad]
double calcHourAngle(double localTime);

//Calculate constant 1
double calcConstantOne(double latitude, double sunDeclination);

//Calculate constant 2
double calcConstantTwo(double latitude, double sunDeclination);

//Convert to degrees
double convertToDegrees(double radian);

//Convert to radians
double convertToRadians(double degree);

#endif

