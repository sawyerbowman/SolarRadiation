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
    
    if (argc != 6){
        printf("Enter a filename to read into the program, a filename to store the output, and"
               "three numbers representing a start time, end time, and step size for time increments.\n");
        return 0;
    }
    
    elevName = args[1];
    energyName = args[2];
    double beginTime = atof(args[3]);
    double endTime = atof(args[4]);
    double timeStep = atof(args[5]);
    printf("%s\n", elevName);
    printf("%s\n", energyName);
    printf("%f\n", beginTime);
    printf("%f\n", endTime);
    printf("%f\n", timeStep);
    
    //Initialize the elevGrid from the file
    Grid* elevGrid;
    elevGrid = readFile(elevName);
    
    //Initialize the viewshedGrid from the elevGrid
    Grid* viewshedGrid;
    viewshedGrid = viewshedGridInit(elevGrid);
    
    //printHeader(viewshedGrid);
    //printValues(viewshedGrid);
    
    //computeViewshed(elevGrid, viewshedGrid, vpi, vpj);
    
    //printHeader(viewshedGrid);
    //printValues(viewshedGrid);
    
    //Save the viewshed grid to the specified file
    //writeFile(viewfname, viewshedGrid);
}