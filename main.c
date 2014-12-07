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
    
    /*
    double sunLong = calcSunLong(12, -5);
    double sunLat = calcSunLat(340);
    
    printf("%lf\n", sunLat);
    printf("%lf\n", sunLong);
    */
    
    
    char *elevName, *energyName;
    
    if (argc != 9){
        printf("Enter a filename to read into the program, a filename to store the output, and"
               "three numbers representing a start time, end time, and step size for time increments.\n");
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
    
    //Initialize the elevGrid from the file
    Grid* elevGrid;
    elevGrid = readFile(elevName);
    
    
    printHeader(elevGrid);
    
    //Initialize the energyGrid from the elevGrid
    Grid* energyGrid;
    energyGrid = gridInit(elevGrid);
    
    computeViewshed(elevGrid, energyGrid, beginTime, endTime, timeStep, dayNum, timeZone, turbidity);
    
    //printHeader(viewshedGrid);
    //printValues(viewshedGrid);
    
    //Save the viewshed grid to the specified file
    writeFile(energyName, energyGrid);
}