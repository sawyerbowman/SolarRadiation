//
//  irradiance.c
//  
//
//  Created by Sawyer Bowman on 12/1/14.
//
//

#include "irradiance.h"
#include "solarPosition.h"

double beamIrradiance(double h, double elevation, double turbidity, double dayNum, double sunAngle){
    double dayAngle = calcDayAngle(dayNum);
    double correctionFactor = calcCorrectionFactor(dayAngle);
    double et = calcEtIrradiance(correctionFactor);
    
    double pRatio = calcP(elevation);
    double deltaHRef = calcDeltaHRef(sunAngle);
    double href = calcHRef(sunAngle, deltaHRef);
    double m = calcM(pRatio, href);
    double rayleigh = calcRayleigh(m);
    double normalIrradiance = calcBNormal(et, turbidity, m, rayleigh);
    
    return calcHorizontalBeamIrradiance(normalIrradiance, sunAngle);
}

//Calculate the Beam Irradiance for horizontal surface
double calcHorizontalBeamIrradiance(double normalIrradiance, double h) {
    return normalIrradiance*sin(h);
}

//Calculate extraterrestrial irradiance
double calcEtIrradiance(double correctionFactor) {
    return SOLAR_CONSTANT*correctionFactor;
}

//Calculate correction factor
double calcCorrectionFactor(double dayAngle) {
    return 1 + .03344 * cos(dayAngle - .048869);
}

//Calculate beam irradiance normal to the solar beam
double calcBNormal(double et, double turbidity, double m, double rayleigh) {
    return et*exp(-.8662*turbidity*m*rayleigh);
}

//Calculate relative optical air mass
double calcM(double pRatio, double href) {
    return pRatio/(sin(href) + .50572*pow((href + 6.07995), -1.6364));
}

//Calculate corrected solar altitude (href)
double calcHRef(double h, double deltaHRef) {
    return h+deltaHRef;
}

//Calculate atmospheric refraction component (∆href)
double calcDeltaHRef(double h) {
    return .061359*(.1594+1.123*h + .065656*pow(h, 2))/(1 + 28.9344*h + 277.3971*pow(h, 2));
}

//Calculate p/p0 component
double calcP(double elevation) {
    return exp(-elevation/8434.5);
}

//Calculate Rayleigh optical thickness
double calcRayleigh(double m) {
    if (m <= 20){
        return 1/(6.6296 + 1.7513*m + .1202*pow(m, 2) + .0065*pow(m, 3) + .00013*pow(m, 4));
    }
    return 1/(10.4+.718*m);
}
