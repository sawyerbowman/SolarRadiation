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

//Retrieve necessary components to calcualte beam irradiance
double beamIrradiance(double h, double elevation, double turbidity, double dayNum, double sunAngle);

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




#endif
