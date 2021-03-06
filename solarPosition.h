//
//  solarPosition.h
//
//
//  Created by Sawyer Bowman on 12/1/14.
//
//

#ifndef _solarPosition_h
#define _solarPosition_h

#define TROPIC_LAT 23.437778 
#define JUNE_21 171

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>

//Calculate the time of the sunrise
double calcSunriseOrSunset(double latitude, double longitude, double dayNum, double hour, int sunrise);

//Determine whether the begin time and end time are within the bounds of sunrise and sunset
int isInDark(double sunrise, double sunset, double beginTime, double endTime);

//Calculate the angle between sun path and horizontal surface
double calcSunAngle(double dayNum, double localTime, double localLat, double elevation, double distance);

//Calculate the latitude under the sun
double calcSunLat(double dayNum);

//Calculate the longitude under the sun
double calcSunLong(double time, double offset);

//Transform the elevation angle based on elevation of particular point
double angleHeightTransform(double sunAngle, double distance, double elevation);

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

//Determine if it is daylight savings, and recalculate given time
void isDayLightSavings(double* beginTime, double* endTime, int daylightSavings);

//Convert time by springing forward to normal time
void convertTime(double* time);

//Flush out user input
void flush();

#endif

