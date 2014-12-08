//
//  irradiance.h
//  
//
//  Created by Sawyer Bowman on 12/1/14.
//
//

#ifndef _irradiance_h
#define _irradiance_h

#define SOLAR_CONSTANT  1367   //in W*m^-2

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <float.h>

/**
 *These methods help compute the energy in GigaJoules over a timestep
 */

//Calculate the gigajoules for a time step given irradiance, cellSizeSquared, and time in seconds
double calcGigaJoulesOverTimeStep(double irradiance, double cellSizeSquared, double timeInSeconds);

//Convert the timestep to seconds
double convertTimestepToSeconds(double timeStep);

/**
 *This method returns the global irradiance
 */

//Calculate the global irradiance
double calcGlobalIrradiance(double elevation, double turbidity, double dayNum, double sunAngle, int visible);

/**
 *The following methods are for beam irradiance
 */

//Retrieve necessary components to calculate beam irradiance
double beamIrradiance(double elevation, double turbidity, double dayNum, double sunAngle);

//Calculate the Beam Irradiance for horizontal surface
double calcHorizontalBeamIrradiance(double normalIrradiance, double h);

//Calculate extraterrestrial irradiance
double calcEtIrradiance(double correctionFactor);

//Calculate correction factor
double calcCorrectionFactor(double dayAngle);

//Calculate beam irradiance normal to the solar beam
double calcBNormal(double et, double turbidity, double m, double rayleigh);

//Calculate relative optical air mass
double calcM(double pRatio, double href);

//Calculate corrected solar altitude (href)
double calcHRef(double h, double deltaHRef);

//Calculate atmospheric refraction component (∆href)
double calcDeltaHRef(double h);

//Calculate p/p0 component
double calcP(double elevation);

//Calculate Rayleigh optical thickness
double calcRayleigh(double m);

/**
 *The following methods are for diffuse irradiance
 */

//Retrieve necessary components to calculate diffuse irradiance
double diffuseIrradiance(double h, double turbidity, double dayNum);

//Calculate diffuse irradiance
double calcDiffuseIrradiance(double et, double transmission, double solarFunction);

//Calculate value of transmission function
double calcTransmission(double turbidity);

//Calculate value of solar altitude function
double calcSolar(double a1, double a2, double a3, double h);

//Calculate A1
double calcA1(double a1Prime, double transmission);

//Calculate A1Prime
double calcA1Prime(double turbidity);

//Calculate A2
double calcA2(double turbidity);

//Calculate A3
double calcA3(double turbidity);



#endif
