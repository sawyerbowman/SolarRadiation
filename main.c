//
//  main.c
//
//
//  Created by Sawyer Bowman on 12/1/14.
//
//

#include "viewshed.h"
#include "solarPosition.h"

int main(int argc, char** args){
    
    char *elevName, *energyName;
    
    if (argc != 9){
        printf("Enter a filename to read into the program (ex maine.asc), "
               "a filename to store the output (ex maine_energy.asc), "
               "a start time (ex 12.5 for 12:30 p.m.), "
               "an end time (ex 17 for 5:00 p.m.), "
               "a step size for time increments (ex 2.5 for 2.5 hours), "
               "a day number (ex 1 for Jan 1), "
               "a UTC time zone offset (ex -5 for EST), "
               "and a turbidity value from the SODA database (ex 2.2, assume uniform across grid.\n");
        return 0;
    }
    
    elevName = args[1];
    energyName = args[2]; 
    double beginTime = atof(args[3]);
    double endTime = atof(args[4]);
    double timeStep = atof(args[5]);
    double dayNum = atof(args[6]);
    double timeZone = atof(args[7]);
    double turbidity = atof(args[8]);
    printf("%s\n", elevName);
    printf("%s\n", energyName);
    printf("%f\n", beginTime);
    printf("%f\n", endTime);
    printf("%f\n", timeStep);
    printf("%f\n", dayNum);
    printf("%f\n", timeZone);
    printf("%f\n", turbidity);
    
    //Ask if daylight savings time, recalculate start and end time accordingly
    isDayLightSavings(&beginTime, &endTime);
    
    //Initialize the elevGrid from the file
    Grid* elevGrid;
    elevGrid = readFile(elevName);
    
    //If it is dark during the time interval, ask user to input times in the day
    double sunrise = calcSunriseOrSunset(elevGrid->latitude, elevGrid->longitude, dayNum, beginTime, 1);
    double sunset = calcSunriseOrSunset(elevGrid->latitude, elevGrid->longitude, dayNum, beginTime, 0);
    
    if (isInDark(sunrise, sunset, beginTime, endTime) == 1){
        printf("Please enter a begin time and an end time that occur during daylight"
               "(after the sun rises at %lf and before the sun sets at %lf\n", sunrise, sunset);
    }
    
    printHeader(elevGrid);
    
    //Initialize the energyGrid from the elevGrid
    Grid* energyGrid;
    energyGrid = gridInit(elevGrid);
    
    computeViewshed(elevGrid, energyGrid, beginTime, endTime, timeStep, dayNum, timeZone, turbidity);
    
    //Save the viewshed grid to the specified file
    writeFile(energyName, energyGrid);
    
}