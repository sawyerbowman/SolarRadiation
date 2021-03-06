//
//  viewshed.h
//
//
//  Created by Sawyer Bowman on 12/1/14.
//
//

#ifndef _viewshed_h
#define _viewshed_h

#define EARTH_RADIUS 6371000
#define ERROR .00000000001
 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <float.h>
//#include <omp.h>
#include <pthread.h>
#include <limits.h>
#include<sys/stat.h>

#include "viewshed.h"
#include "solarPosition.h"
#include "irradiance.h"

typedef struct _grid {
    //The size of the grid and other header properties
    int rows, cols;
    double longitude, latitude, cellsize, noDataValue;
    
    //The 2D array of values in the grid
    float** data;
}Grid;

typedef struct ThreadData {
    Grid* elevGrid;
    Grid* viewshedGrid;
    Grid* energyGrid;
    double startTime, endTime, timeStep, dayNum, timeZone, turbidity, sunLat, sunLong;
    int i, endi;
}ThreadData;

//Computes whether (i,j) is visible from (vprow, vpcol)
int pointVisibleFromSun(Grid* elevGrid, Grid* energyGrid, double currentLat, double currentLong, int i, int j, double sunLat, double sunLong, double dayNum, double localTime, double turbidity, double timeStep);

//Compute the viewshed
void computeViewshed(Grid* elevGrid, Grid* energyGrid, double startTime, double endTime, double timeStep, double dayNum, double timeZone, double turbidity, int numThreads);

//Parallelizable viewshed computations
void* viewshedLoops(struct ThreadData* data);

//Reads information (terrain) from a given file
Grid* readFile(char* name);

//Writes to the specified file
void writeFile(char* name, Grid* grid);

//Prints the grid (header)
void printHeader(Grid* g);

//Prints the grid (values)
void printValues(Grid* g);

//Initialize the ViewShed Grid
Grid* gridInit(Grid* elevGrid);

//Calculate slope given 2 points
float calculateSlope(float startx, float starty, float endx, float endy);

//Calculate the distance between 2 given points
float calculateDistance(float startx, float starty, float endx, float endy);

//Calculate the haversine distance between 2 points
double calcHaversine(double startx, double starty, double endx, double endy);

//Replace sunLat, sunLong with intersection of line with edge of grid
int findNewEndPoint(Grid* grid, double* endx, double* endy, double sunLat, double sunLong, double originalSlope, double originalIntercept);

//Create a random grid
void createRandomGrid(int rows, int cols, double longitude, double latitude, double cellsize, double nodata, int maxValue);

//Create an ordered grid
void createOrderedGrid(int rows, int cols, double longitude, double latitude, double cellsize, double nodata, int minValue, int maxValue);

//Calculate x coordinate of intersection between two lines
//double calcXIntersection(double interceptB, double interceptA, double slopeA, double slopeB);

//Calculate y coordinate of intersection between two lines
//double calcYIntersection(double slope, double intercept, double x);

//Convert latitude to i coordinate
//double convertLatToI(double latitude, Grid* grid);

//Convert longitude to j coordinate
//double convertLongToJ(double longitude, Grid* grid);

//Convert i coordinate to latitude
//double convertIToLat(double i, Grid* grid);

//Convert j coordinate to longitude
//double convertJToLong(double j, Grid* grid);

//Determine if intersection coordinate is located on line between two corners of grid
int isOnLine(double intersection, double cornerOne, double cornerTwo);

#endif
