//
//  viewshed.c
//
//
//  Created by Sawyer Bowman on 12/1/14.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "viewshed.h"

/**
 *Read data from the file into the grid
 */

Grid* readFile(char* name){
    FILE *f;
    char s[100];
    int ncols, nrows, cellsize, longitude, latitude, noDataValue;
    
    f = fopen(name, "r");
    if (f == NULL){
        printf("Cannot open file!");
        exit(1);
    }
    
    //First get the header (first 6 lines of file)
    int i;
    for (i = 0; i < 6; i++){
        fscanf(f, "%s", s);
        if (strcmp(s, "ncols") == 0){
            fscanf(f, "%d", &ncols);
        }
        else if (strcmp(s, "nrows") == 0){
            fscanf(f, "%d", &nrows);
        }
        else if (strcmp(s, "xllcorner") == 0){
            fscanf(f, "%d", &longitude);
        }
        else if (strcmp(s, "yllcorner") == 0){
            fscanf(f, "%d", &latitude);
        }
        else if (strcmp(s, "cellsize") == 0){
            fscanf(f, "%d", &cellsize);
        }
        else if (strcmp(s, "NODATA_value") == 0){
            fscanf(f, "%d", &noDataValue);
        }
        else {
            printf("Invalid format\n");
            break;
        }
    }
    
    //Now that we know the size, read in the data
    float** data;
    data = (float**)malloc(nrows * sizeof(float*));
    int row, col;
    for (row = 0; row < nrows; row++){
        data[row] = (float*)malloc(ncols * sizeof(float));
    }
    
    for (row = 0; row < nrows; row++){
        for (col = 0; col < ncols; col++){
            fscanf(f, " %f", &data[row][col]);
        }
    }
    
    //Create the grid and return it
    Grid* grid;
    grid = malloc(sizeof(Grid));
    grid->rows = nrows;
    grid->cols = ncols;
    grid->longitude = longitude;
    grid->latitude = latitude;
    grid->cellsize = cellsize;
    grid->noDataValue = noDataValue;
    grid->data = data;
    
    return grid;
}

/**
 *Write the data to the specified file name
 */

void writeFile(char* name, Grid* grid){
    FILE* file = fopen(name, "ab+");
    char* ncols = "ncols ";
    char* nrows = "nrows ";
    char* longitude = "xllcorner ";
    char* latitude = "yllcorner ";
    char* cellsize = "cellsize ";
    char* noDataValue = "NODATA_value ";
    fprintf(file, "%s", ncols);
    fprintf(file, "%d\n", grid->cols);
    fprintf(file, "%s", nrows);
    fprintf(file, "%d\n", grid->rows);
    fprintf(file, "%s", xllcorner);
    fprintf(file, "%d\n", grid->longitude);
    fprintf(file, "%s", yllcorner);
    fprintf(file, "%d\n", grid->latitude);
    fprintf(file, "%s", cellsize);
    fprintf(file, "%d\n", grid->cellsize);
    fprintf(file, "%s", noDataValue);
    fprintf(file, "%d\n", grid->noDataValue);
    
    int i, j;
    for (i = 0; i < grid->rows; i++){
        for (j = 0; j < grid->cols; j++){
            fprintf(file, "%f ", grid->data[i][j]);
        }
        fprintf(file, "\n");
    }
    printf("DONE!\n");
}

/**
 *Given a grid, print its header
 */

void printHeader(Grid* grid){
    printf("ncols: \t\t %d\n", grid->cols);
    printf("nrows: \t\t %d\n", grid->rows);
    printf("longitude: \t %d\n", grid->longitude);
    printf("latitude: \t %d\n", grid->latitude);
    printf("cellsize: \t %d\n", grid->cellsize);
    printf("NODATA_value: \t %d\n", grid->noDataValue);
}

/**
 *Given a grid, print its values
 */

void printValues(Grid* grid){
    float** data = grid->data;
    int i, j;
    for (i = 0; i < grid->rows; i++){
        for (j = 0; j < grid->cols; j++){
            printf("%f ", data[i][j]);
        }
        printf("\n");
    }
}

/**
 *Initialize a viewshed grid
 */

Grid* viewshedGridInit(Grid* elevGrid){
    float** data = (float**)malloc(elevGrid->rows*sizeof(float*));
    int i, j;
    for (i = 0; i < elevGrid->rows; i++){
        data[i] = (float*)malloc(elevGrid->cols*sizeof(float));
    }
    
    for (i = 0; i < elevGrid->rows; i++){
        for (j = 0; j < elevGrid->cols; j++){
            if (elevGrid->data[i][j] == elevGrid->noDataValue){
                data[i][j] = elevGrid->noDataValue;
            }
            else {
                data[i][j] = 0;
            }
        }
    }
    
    Grid* viewshedGrid = malloc(sizeof(Grid));
    viewshedGrid->rows = elevGrid->rows;
    viewshedGrid->cols = elevGrid->cols;
    viewshedGrid->xllcorner = elevGrid->xllcorner;
    viewshedGrid->yllcorner = elevGrid->yllcorner;
    viewshedGrid->cellsize = elevGrid->cellsize;
    viewshedGrid->noDataValue = elevGrid->noDataValue;
    viewshedGrid->data = data;
    
    return viewshedGrid;
}

/**
 *Calculates the slope given 2 points
 */

float calculateSlope(float startx, float starty, float endx, float endy){
    float slope;
    float xDifference = endx-startx;
    //Account for floating point error (divide by 0)
    if (xDifference == 0){
        //Use the maximum possible value to simulate a slope of infinity
        slope = FLT_MAX;
    }
    else {
        //slope is inverted because left upper corner is (0,0)
        slope = ((endy-starty)/(xDifference))*-1;
    }
    return slope;
}

/**
 *Implements the distance formula
 */

float calculateDistance(float startx, float starty, float endx, float endy){
    float xTotal = (startx-endx)*(startx-endx);
    float yTotal = (starty-endy)*(starty-endy);
    float distance = sqrt(xTotal+yTotal);
    return distance;
}

/**
 *Find end point that intersects with a particular edge of the grid
 */

void findNewEndPoint(Grid* grid, double* endx, double* endy, double sunLat, double sunLong, double originalSlope, double originalIntercept){
    //Determine which grid edge the line running between the sun and current point
    //intersects first by calculating corners (starting at lower left and moving
    //clockwise
    //double cornerOneLat = grid->latitude;
    //double cornerOneLong = grid->longitude;
    
    double cornerTwoLat = convertIToLat(grid->rows, grid);
    double cornerTwoLong = grid->longitude;
    
    //double cornerThreeLat = converTwoLat;
    //double cornerThreeLong = convertJToLong(grid->cols, grid);
    
    double cornerFourLat = cornerOneLat;
    double cornerFourLong = cornerThreeLong;
    
    //Horizontal lines (one is bottom, two is top of grid), slope of zero
    double slopeOne = 0;
    double interceptOne = grid->latitude;
    double slopeTwo = 0;
    double interceptThree = convertIToLat(0);
    
    //Vertical lines (two is left, four is right), slope of biggest float
    double slopeTwo = FLT_MAX;
    double interceptTwo = cornerTwoLat - slopeTwo*cornerTwoLong;
    double slopeFour = FLT_MAX;
    double interceptFour = cornerFourLat - slopeFour*cornerFourLong;
    
    //TODO: These bits of code could be combined into function, have to account for changes though...
    //Determine if intersection with bottom grid border is correct
    double xOne = calcXIntersection(interceptOne, originalIntercept, originalSlope, slopeOne);
    double yOne = calcYIntersection(originalSlope, originalIntercept, xOne);
    if (originalSlope < 0 && endy >= yOne && endx < xOne){
        endy = yOne;
        endx = xOne;
        return;
    }
    else if (originalSlope >= 0 && endy >= yOne && endx >= xOne){
        endy = yOne;
        endx = xOne;
        return;
    }
    
    //determine if intersection with left grid border is correct
    double xTwo = calcXIntersection(interceptTwo, originalIntercept, originalSlope, slopeTwo);
    double yTwo = calcYIntersection(originalSlope, originalIntercept, xTwo);
    if (originalSlope < 0 && endy < yTwo && endx >= xTwo){
        endy = yTwo;
        endx = xTwo;
        return;
    }
    else if (originalSlope >= 0 && endy >= yTwo && endx >= xTwo){
        endy = yTwo;
        endx = xTwo;
        return;
    }
    
    //determine if intersection with top grid border is correct
    double xThree = calcXIntersection(interceptThree, originalIntercept, originalSlope, slopeThree);
    double yThree = calcYIntersection(originalSlope, originalIntercept, xThree);
    if (originalSlope < 0 && endy <= yThree && endx >= xThree){
        endy = yThree;
        endx = xThree;
        return;
    }
    else if (originalSlope >= 0 && endy <= yThree && endx < xThree){
        endy = yThree;
        endx = xThree;
        return;
    }
    
    //determine if intersection with right grid border is correct
    double xFour = calcXIntersection(interceptFour, originalIntercept, originalSlope, slopeFour);
    double yFour = calcYIntersection(originalSlope, originalIntercept, xFour);
    if (originalSlope < 0 && endy >= yFour && endx < xFour){
        endy = yFour;
        endx = xFour;
        return;
    }
    else if (originalSlope >= 0 && endy < yFour && endx < xFour){
        endy = yFour;
        endx = xFour;
        return;
    }
}

/**
 *Find x coordinate of intersection between two lines
 */

double calcXIntersection(double interceptB, double interceptA, double slopeA, double slopeB){
    return (interceptB-interceptA)/(slopeA-slopeB);
}

/**
 *Find y coordinate of intersection between two lines
 */

double calcYIntersection(double slope, double intercept, double x){
    return (slope*x)+intercept;
}

/**
 *Convert latitude to i (y)
 */
double convertLatToI(double latitude, Grid* grid){
    return (latitude - grid->latitude)/(grid->cellsize * grid->rows);
}

/**
 *Convert longitude to j (x)
 */
double convertLongToJ(double longitude, Grid* grid){
    return (longitude - grid->longitude)/grid->cellsize;
}

/**
 *Convert i (y) to latitude
 */
double convertIToLat(double i, Grid* grid){
    return grid->latitude + grid->cellsize*(grid->rows*i);
}

/**
 *Convert j (x) to longitude
 */
double convertJToLong(double j, Grid* grid){
    return grid->longitude + grid->cellsize*j;
}

/**
 *Computes whether (i,j) represented by currentLat, currentLong, is visible from the point directly
 *under the sun. Throughout this method, it is pivotal to remember that y is row (i) and x is col (j)
 *A lot of the commented print statements display different variable values, useful for debugging
 */

int pointVisibleFromSun(Grid* elevGrid, double currentLat, double currentLong, int i, int j, double sunLat, double sunLong){
    //printf("\n");
    //printf("Checking if New Point (%d, %d) is visible from Original Point (%d, %d)\n", i, j, vpi, vpj);
    
    //use visible as a boolean variable, 1 represents visible, 0 not visible
    int visible = 1;
    float slope;
    float intercept;
    
    float newHeight = elevGrid->data[i][j];
    
    //Use these points (start and end) to make sure lines always run from left to right
    //Later, we will use these same points to make sure lines run from bottom to top
    float startx;
    float starty;
    float endx;
    float endy;
    
    //The point (i,j) represented by (currentLat, currentLong) is farther right than (sunLat, sunLong),
    //make (i,j) represented by (currentLat, currentLong) the end point
    if (currentLong >= sunLong){
        startx = sunLong;
        starty = sunLat;
        endx = currentLong;
        endy = currentLat;
    }
    //The (sunLat, sunLong) is farther right than (i,j) represented by (currentLat, currentLong),
    //make (sunLat, sunLong) the end point
    else {
        startx = currentLong;
        starty = currentLat;
        endx = sunLong;
        endy = sunLat;
    }
    
    //Row corresponds to y, Col corresponds to x
    //printf("Start Point: (%f, %f)\n", starty, startx);
    //printf("End Point: (%f, %f)\n", endy, endx);
    
    slope = calculateSlope(startx, starty, endx, endy);
    //printf("Slope: (%f)\n", slope);
    
    //intercept is y - slope*x = b
    intercept = starty - (-1*slope*startx);
    //printf("Intercept: (%f)\n", intercept);
    
    //We want to replace sunLat and sunLong with a newly calculated point that intersects at the edge of the grid
    double* newX, newY;
    if (endx == sunLat){
        newX = startx;
        newY = starty;
    }
    else {
        newX = endx;
        newY = endy;
    }
    
    findNewEndPoint(elevGrid, newX, newY, sunLat, sunLong, slope, intercept);
    
    if (endx == sunLat){
        startx = newX;
        starty = newY;
    }
    else {
        endx = newX;
        endy = newY;
    }
    
    //xAxis is the variable that is incremented by 1 unit each time
    //It represents the intersection point's x value
    int xAxis = startx+1;
    
    //printf("X Checks\n");
    
    for (; xAxis < endx; xAxis++){
        //printf("xAxis: (%d)\n", xAxis);
        //intersectY represents the intersection point's y value (needs to be inverted)
        float intersectY = (-1*slope*xAxis) + intercept;
        //printf("Intersection Point's y value: (%f)\n", intersectY);
        //h1y represents the y value of the point below the intersection point
        int h1y = ceil(intersectY);
        //h2y represents the y value of the point above the intersection point
        int h2y = floor(intersectY);
        //elevH1 is the elevation of the h1 point
        float elevH1 = elevGrid->data[h1y][xAxis];
        //printf("Point 1 (%d, %d) Elevation: (%f)\n", h2y, xAxis, elevH1);
        //elevH2 is the elevation of the h2 point
        float elevH2 = elevGrid->data[h2y][xAxis];
        //printf("Point 2 (%d, %d) Elevation: (%f)\n", h1y, xAxis, elevH2);
        //elevAngle is the angle of elevation between the h1 and h2 points
        float elevAngle;
        float elevDifference;
        float lowerHeight;
        if (elevH2 > elevH1){
            elevDifference = elevH2-elevH1;
            lowerHeight = elevH1;
        }
        else {
            elevDifference = elevH1-elevH2;
            lowerHeight = elevH2;
        }
        
        //Calculate the elevation angle between h1 and h2 (distance between them is 1)
        elevAngle = atan((elevDifference)/1);//elevGrid->cellsize
        //printf("Elevation angle: (%f)\n", elevAngle);
        
        //pY is the relevant decimal of the intersect point's y value
        float pY = intersectY-floor(intersectY);
        //intersectHeight is the elevation of the intersection point
        float intersectHeight = lowerHeight + (pY*tan(elevAngle));
        //printf("Intersection Point height: (%f)\n", intersectHeight);
        //printf("Original Point height: (%f)\n", originalHeight);
        
        //If the angle of elevation between the original point and the intersect point is greater
        //than the angle of elevation between the original point and the new point, the view path
        //is obstructed
        
        //Angle of elevation is arctan(opposite/adjacent) where opposite is difference in height
        //and adjacent is distance between points
        //printf("Distance between Original Point and Intersect is (%f)\n", calculateDistance(vpi, vpj, intersectY, xAxis));
        
        double solarAngleIntersection = calcSunAngle(dayNum, localTime, convertIToLat(intersectY), intersectHeight, calculateDistance(sunLat, sunLong, intersectY, xAxis));
        
        //float angleOriginalIntersection = atan((fabs(originalHeight-intersectHeight))/calculateDistance(vpi, vpj, intersectY, xAxis));
        //If the Original Point is higher than the Intersection point, the elevation angle must be negative
        //if (originalHeight > intersectHeight){
            //angleOriginalIntersection *= -1;
        //}
        //printf("Distance between Original Point and New Point is (%f)\n", calculateDistance(vpi, vpj, i, j));
        
        double solarAngleNew = calcSunAngle(dayNum, localTime, convertIToLat(i), newHeight, calculateDistance(sunLat, sunLong, currentLat, currentLong));
        
        //float angleOriginalNew = atan((fabs(originalHeight-newHeight))/calculateDistance(vpi, vpj, i, j));
        //If the Original Point is higher than the New Point, the elevation angle must be negative
        //if (originalHeight > newHeight){
            //angleOriginalNew *= -1;
        //}
        //printf("Angle of elevation between Original Point and Intersection Point is (%f)\n", angleOriginalIntersection);
        //printf("Angle of elevation between Original Point and New Point is (%f)\n", angleOriginalNew);
        if (solarAngleIntersection > solarAngleNew){
            //printf("New Point (%d, %d) is not visible from Original Point (%d, %d)\n", i, j, vpi, vpj);
            return 0;
        }
    }
    
    /**
     *TODO: Fix the second half to mimic first half
     */
    
    //Make sure lines always run from bottom to top
    if (i < vpi){
        startx = vpj;
        starty = vpi;
        endx = j;
        endy = i;
    }
    else {
        startx = j;
        starty = i;
        endx = vpj;
        endy = vpi;
    }
    
    //recalculate the slope and intercept (points may have changed)
    slope = calculateSlope(startx, starty, endx, endy);
    //printf("Slope during y check: (%f)\n", slope);
    //Account for case of infinite slope
    if (slope == FLT_MAX){
        intercept = FLT_MAX;
    }
    else {
        intercept = starty - (-1*slope*startx);
    }
    //printf("Intercept: (%f)\n", intercept);
    
    //Row corresponds to y, Col corresponds to x
    //printf("Start Point for y checks (%f %f)\n", starty, startx);
    //printf("End Point for y checks (%f %f)\n", endy, endx);
    
    //The process below is similar to that above, except the y is incremented
    //and the intersections point's x value is calculated
    int yAxis = starty-1;
    
    //printf("Y Checks\n");
    
    for (; yAxis > endy; yAxis--){
        //printf("yAxis: (%d)\n", yAxis);
        float intersectX;
        if (slope == FLT_MAX){
            intersectX = vpj;
        }
        else {
            intersectX = (yAxis-intercept)/slope*-1;
        }
        //float intersectX = (yAxis-intercept)/slope*-1;
        //printf("intersectX: (%f)\n", intersectX); fflush(stdout);
        int h1x = ceil(intersectX);
        int h2x = floor(intersectX);
        float elevH1 = elevGrid->data[yAxis][h1x];
        //printf("Point 1 (%d, %d) elevation (%f)\n", yAxis, h1x, elevH1);
        float elevH2 = elevGrid->data[yAxis][h2x];
        //printf("Point 2 (%d, %d) elevation (%f)\n", yAxis, h2x, elevH2);
        float elevAngle;
        float elevDifference;
        float lowerHeight;
        if (elevH2 > elevH1){
            elevDifference = elevH2-elevH1;
            lowerHeight = elevH1;
        }
        else {
            elevDifference = elevH1-elevH2;
            lowerHeight = elevH2;
        }
        
        elevAngle = atan((elevDifference)/1);
        //printf("Elevation angle (%f)\n", elevAngle);
        
        float pX = intersectX-floor(intersectX);
        //printf("pX: (%f)\n", pX);
        float intersectHeight = lowerHeight + (pX*tan(elevAngle));
        //printf("Intersection point's height (%f)\n", intersectHeight);
        //printf("Original point's height (%f)\n", originalHeight);
        //printf("Distance between original and intersect is (%f)\n", calculateDistance(vpi, vpj, yAxis, intersectX));
        float angleOriginalIntersection = atan((fabs(originalHeight-intersectHeight))/calculateDistance(vpi, vpj, yAxis, intersectX));
        //If the Original Point is higher than the intersection point, the angle of elevation must be negative
        //printf("Original point's height (%f)\n", originalHeight);
        if (originalHeight > intersectHeight){
            angleOriginalIntersection *= -1;
        }
        //printf("Angle of elevation between Original Point and Intersection Point is (%f)\n", angleOriginalIntersection);
        float angleOriginalNew = atan((fabs(originalHeight-newHeight))/calculateDistance(vpi, vpj, i, j));
        //If the Original Point is higher than the new point, the angle of elevation must be negative
        if (originalHeight > newHeight){
            angleOriginalNew *= -1;
        }
        //printf("Distance between original and new is (%f)\n", calculateDistance(vpi, vpj, i, j));
        //printf("Angle of elevation between Original Point and New Point is (%f)\n", angleOriginalNew);
        if (angleOriginalIntersection > angleOriginalNew){
            //printf("New Point (%d, %d) is not visible from Original Point (%d, %d)\n", i, j, vpi, vpj);
            return 0;
        }
    }
    //printf("New Point (%d, %d) is visible from Original Point (%d, %d)\n", i, j, vpi, vpj);
    return visible;
}

/**
 *Compute the viewshed
 */
void computeViewshed(Grid* elevGrid, Grid* viewshedGrid, double startTime, double endTime, double timeStep, double dayNum, double timeZone){
    
    for (startTime; startTime <= endTime; startTime += timeStep){
        //calculate lat and long of position directly under sun
        sunLat = calcSunLat(dayNum);
        sunLong = calcSunLong(startTime, timeZone);
        for (i = 0; i < elevGrid->rows; i++){
            for (j = 0; j < elevGrid->cols; j++){
                //Calculate lat and long of current cell from bottom left corner of grid
                double currentLat = convertIToLat(i, elevGrid);
                //double currentLat = elevGrid->latitude + elevGrid->cellsize*(elevGrid->rows-i);
                double currentLong = convertJToLong(j, elevGrid);
                //double currentLong = elevGrid->longitude + elevGrid->cellsize*j;
                
                if (viewshedGrid->data[i][j] == viewshedGrid->noDataValue){
                    //viewshedGrid->data[i][j] = 0;
                    continue;
                }
                //see if visible (if point directly under sun), if yes set appropriate cell to 1
                if (currentLat == sunLat && currentLong = sunLong){
                    viewshedGrid->data[i][j] = 1;
                    continue;
                }
                
                int visible = pointVisibleFromSun(elevGrid, currentLat, currentLong, i, j, sunLat, sunLong);
                
                if (visible == 1){
                    viewshedGrid->data[i][j] = 1;
                }
                else {
                    viewshedGrid->data[i][j] = 0;
                }
            }
        }
    }
}
