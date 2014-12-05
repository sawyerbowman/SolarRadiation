//
//  irradiance.c
//  
//
//  Created by Sawyer Bowman on 12/1/14.
//
//

#include "irradiance.h"
#include "solarPosition.h"

/**
 *This method uses both beam and diffuse irradiance to calculate
 *the global irradiance
 */

double calcGlobalIrradiance(double elevation, double turbidity, double dayNum, double sunAngle, int visible){
    /*
     *If square is in shadow, it can't have beam irradiance. Regardless of shadow, diffuse
     *irradiance will touch the square.
     */
    double beam, diffuse;
    if (visible == 1){
        beam = beamIrradiance(elevation, turbidity, dayNum, sunAngle);
    }
    else {
        beam = 0;
    }
    diffuse = diffuseIrradiance(sunAngle, turbidity, dayNum);

    return beam + diffuse;
}

/**
 *The following methods are for beam irradiance
 */

double beamIrradiance(double elevation, double turbidity, double dayNum, double sunAngle){
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

/**
 *Calculate the Beam Irradiance for horizontal surface
 */

double calcHorizontalBeamIrradiance(double normalIrradiance, double h) {
    return normalIrradiance*sin(h);
}

/**
 *Calculate extraterrestrial irradiance
 */

double calcEtIrradiance(double correctionFactor) {
    return SOLAR_CONSTANT*correctionFactor;
}

/**
 *Calculate correction factor
 */

double calcCorrectionFactor(double dayAngle) {
    return 1 + .03344 * cos(dayAngle - .048869);
}

/**
 *Calculate beam irradiance normal to the solar beam
 */
 
double calcBNormal(double et, double turbidity, double m, double rayleigh) {
    return et*exp(-.8662*turbidity*m*rayleigh);
}

/**
 *Calculate relative optical air mass
 */

double calcM(double pRatio, double href) {
    return pRatio/(sin(href) + .50572*pow((href + 6.07995), -1.6364));
}

/**
 *Calculate corrected solar altitude (href)
 */

double calcHRef(double h, double deltaHRef) {
    return h+deltaHRef;
}

/**
 *Calculate atmospheric refraction component (∆href)
 */

double calcDeltaHRef(double h) {
    return .061359*(.1594+1.123*h + .065656*pow(h, 2))/(1 + 28.9344*h + 277.3971*pow(h, 2));
}

/**
 *Calculate p/p0 component
 */

double calcP(double elevation) {
    return exp(-elevation/8434.5);
}

/**
 *Calculate Rayleigh optical thickness
 */

double calcRayleigh(double m) {
    if (m <= 20){
        return 1/(6.6296 + 1.7513*m + .1202*pow(m, 2) + .0065*pow(m, 3) + .00013*pow(m, 4));
    }
    return 1/(10.4+.718*m);
}

/**
 *The following methods are for diffuse irradiation
 */

/**
 *Retrieve necessary components to calculate diffuse irradiance
 */

double diffuseIrradiance(double h, double turbidity, double dayNum) {
    double dayAngle = calcDayAngle(dayNum);
    double correctionFactor = calcCorrectionFactor(dayAngle);
    double et = calcEtIrradiance(correctionFactor);
    
    double transmission = calcTransmission(turbidity);
    
    double a1Prime = calcA1Prime(turbidity);
    double a1 = calcA1(a1Prime, transmission);
    double a2 = calcA2(turbidity);
    double a3 = calcA3(turbidity);
    double solar = calcSolar(a1, a2, a3, h);
    
    return calcDiffuseIrradiance(et, transmission, solar);
}

/**
 *Calculate diffuse irradiance
 */
double calcDiffuseIrradiance(double et, double transmission, double solar) {
    return et*transmission*solar;
}

/**
 *Calculate value of transmission function
 */

double calcTransmission(double turbidity) {
    return -.015843 + .030543*turbidity + .0003797*pow(turbidity, 2);
}

/**
 *Calculate value of solar altitude function
 */

double calcSolar(double a1, double a2, double a3, double h) {
    return a1 + a2*sin(h) + a3*pow(sin(h), 2);
}

/**
 *Calculate A1
 */

double calcA1(double a1Prime, double transmission) {
    if (a1Prime*transmission < .0022){
        return .0022/transmission;
    }
    return a1Prime;
}

/**
 *Calculate A1Prime
 */

double calcA1Prime(double turbidity) {
    return .26463 - .061581*turbidity + .0031408*pow(turbidity, 2);
}

/**
 *Calculate A2
 */

double calcA2(double turbidity) {
    return 2.04020 + .018945*turbidity -.011161*pow(turbidity, 2);
}

/**
 *Calculate A3
 */

double calcA3(double turbidity) {
    return -1.3025 + .039231*turbidity + .0085079*pow(turbidity, 2);
}

