//
//  viewshed.h
//
//
//  Created by Sawyer Bowman on 12/1/14.
//
//

#ifndef _viewshed_h
#define _viewshed_h
#include "solarPosition.h"

typedef struct _grid {
  //The size of the grid and other header properties
  int rows, cols, longitude, latitude, cellsize, noDataValue;
  
  //The 2D array of values in the grid
  float** data;
}Grid;

//Computes whether (i,j) is visible from (vprow, vpcol)
int pointVisibleFromSun(Grid* elevGrid, double currentLat, double currentLong, int i, int j, double sunLat, double sunLong);

//Compute the viewshed
void computeViewshed(Grid* elevGrid, Grid* viewshedGrid, double startTime, double endTime, double timeStep, double dayNum, double timeZone);

//Reads information (terrain) from a given file
Grid* readFile(char* name);

//Writes to the specified file
void writeFile(char* name, Grid* grid);

//Prints the grid (header)
void printHeader(Grid* g);

//Prints the grid (values)
void printValues(Grid* g);

//Initialize the ViewShed Grid
Grid* viewshedGridInit(Grid* elevGrid);

//Calculate slope given 2 points
float calculateSlope(float startx, float starty, float endx, float endy);

//Calculate the distance between 2 given points
float calculateDistance(float startx, float starty, float endx, float endy);

#endif