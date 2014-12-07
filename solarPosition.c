//
//  solarPosition.c
//
//
//  Created by Sawyer Bowman on 12/1/14.
//
//

#include "solarPosition.h"

/**
 *Calculate the angle between sun path and horizontal surface
 *Assume localLat is in degrees (most common form of latitude).
 *Must convert to radians. Returns in degrees.
 */

double calcSunAngle(double dayNum, double localTime, double localLat, double elevation, double distance){
    double latRadian = convertToRadians(localLat);
    double dayAngle = calcDayAngle(dayNum);
    double sunDeclination = calcSunDeclination(dayAngle);
    double constOne = calcConstantOne(latRadian, sunDeclination);
    double constTwo = calcConstantTwo(latRadian, sunDeclination);
    double hourAngle = calcHourAngle(localTime);
    double sunAngleRad = asin(constOne*cos(hourAngle) + constTwo);
    double correctAngleRad = angleHeightTransform(sunAngleRad, distance, elevation);
    return convertToDegrees(correctAngleRad);
}

/**
 *Calculate latitude of position directly under sun. Returns in degrees.
 */

double calcSunLat(double dayNum){
    return TROPIC_LAT*cos((dayNum-JUNE_21)/(365.25/(2*M_PI)));
}

/**
 *Calculate longitude of position directly under sun
 */

double calcSunLong(double time, double offset){
    int ut = ((int) time - (int) offset + 24) % 24;
    return (ut - 12)*-360/24;
}

/**
 *Apply transformation of angle of elevation for point's elevation
 */

double angleHeightTransform(double sunAngle, double distance, double elevation){
    double corrected = atan(tan(sunAngle) - (elevation/distance));
    return corrected;
}

/**
 *Calculate day angle [rad]
 */

double calcDayAngle(double j){
    return (M_PI*2*j)/365.25;
}

/**
 *Calculate sun declination [rad]
 */

double calcSunDeclination(double dayAngle){
    return asin(.3978*sin(dayAngle-1.4+.0355*sin(dayAngle-.0489)));
}

/**
 *Calculate hour angle [rad] (localTime on a 24 hour clock)
 */

double calcHourAngle(double localTime){
    return (.261799*(localTime-12));
}

/**
 *Calculate constant 1
 */

double calcConstantOne(double latitude, double sunDeclination){
    return cos(latitude)*cos(sunDeclination);
}

/**
 *Calculate constant 2
 */

double calcConstantTwo(double latitude, double sunDeclination){
    return sin(latitude)*sin(sunDeclination);
}

/**
 *Convert radians to degrees
 */

double convertToDegrees(double radian){
    return (radian*180)/M_PI;
}

/**
 *Convert degrees to radians
 */

double convertToRadians(double degree){
    return degree*(M_PI/180);
}