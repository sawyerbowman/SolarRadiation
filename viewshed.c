//
//  viewshed.c
//
//
//  Created by Sawyer Bowman on 12/1/14.
//
//

#include "viewshed.h"

static inline double calcXIntersection(double interceptB, double interceptA, double slopeA, double slopeB);

static inline double calcYIntersection(double slope, double intercept, double x);

static inline double convertLatToI(double latitude, Grid* grid);

static inline double convertLongToJ(double longitude, Grid* grid);

static inline double convertIToLat(double i, Grid* grid);

static inline double convertJToLong(double j, Grid* grid);

/**
 *Read data from the file into the grid
 */

Grid* readFile(char* name){
    FILE *f;
    char s[100];
    int ncols, nrows;
    double cellsize, longitude, latitude, noDataValue;
    
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
            fscanf(f, "%lf", &longitude);
        }
        else if (strcmp(s, "yllcorner") == 0){
            fscanf(f, "%lf", &latitude);
        }
        else if (strcmp(s, "cellsize") == 0){
            fscanf(f, "%lf", &cellsize);
        }
        else if (strcmp(s, "NODATA_value") == 0){
            fscanf(f, "%lf", &noDataValue);
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
    FILE* file = fopen(name, "w+");
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
    fprintf(file, "%s", longitude);
    fprintf(file, "%f\n", grid->longitude);
    fprintf(file, "%s", latitude);
    fprintf(file, "%f\n", grid->latitude);
    fprintf(file, "%s", cellsize);
    fprintf(file, "%f\n", grid->cellsize);
    fprintf(file, "%s", noDataValue);
    fprintf(file, "%f\n", grid->noDataValue);
    
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
    printf("longitude: \t %f\n", grid->longitude);
    printf("latitude: \t %f\n", grid->latitude);
    printf("cellsize: \t %f\n", grid->cellsize);
    printf("NODATA_value: \t %f\n", grid->noDataValue);
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
 *Create a random grid
 */
void createRandomGrid(int rows, int cols, double longitude, double latitude, double cellsize, double nodata, int maxValue){
    float** data = (float**)malloc(rows*sizeof(float*));
    int i, j;
    for (i = 0; i < rows; i++){
        data[i] = (float*)malloc(cols*sizeof(float));
    }
    
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            int valid = rand() % 20;
            if (valid == 1){
                data[i][j] = nodata;
            }
            else {
                data[i][j] = rand() % maxValue;
            }
        }
    }
    
    Grid* grid = malloc(sizeof(Grid));
    grid->rows = rows;
    grid->cols = cols;
    grid->longitude = longitude;
    grid->latitude = latitude;
    grid->cellsize = cellsize;
    grid->noDataValue = nodata;
    grid->data = data;
    
    writeFile("randomGrid.asc", grid);
}

/**
 *Initialize a grid
 */

Grid* gridInit(Grid* elevGrid){
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
    
    Grid* grid = malloc(sizeof(Grid));
    grid->rows = elevGrid->rows;
    grid->cols = elevGrid->cols;
    grid->longitude = elevGrid->longitude;
    grid->latitude = elevGrid->latitude;
    grid->cellsize = elevGrid->cellsize;
    grid->noDataValue = elevGrid->noDataValue;
    grid->data = data;
    
    return grid;
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
        //No need to invert since we are using latitude and longitude
        slope = ((endy-starty)/(xDifference));
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
 *Implements the Haversine Formula for computing great circle distance between two points
 */

double calcHaversine(double startx, double starty, double endx, double endy){
    double long1Rad = convertToRadians(startx);
    double lat1Rad = convertToRadians(starty);
    double long2Rad = convertToRadians(endx);
    double lat2Rad = convertToRadians(endy);
    
    double deltaLong = long2Rad-long1Rad;
    double deltaLat = lat2Rad-lat1Rad;
    
    double a = sin(deltaLat/2) * sin(deltaLat/2) + cos(lat1Rad) * cos(lat2Rad) * sin(deltaLong/2) * sin(deltaLong/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return EARTH_RADIUS * c;
}

/**
 *Determine if the intersection coordinate is between two given points (latitude or longitude
 */
int isOnLine(double intersection, double cornerOne, double cornerTwo){
    if (intersection >= cornerOne && intersection <= cornerTwo){
        return 1;
    }
    return 0;
}

/**
 *Find end point that intersects with a particular edge of the grid.
 *Also returns an integer (1 for horizontal line intersection, 0 for
 *vertical line intersection). This will help determined whether lat
 *or long is off
 */

int findNewEndPoint(Grid* grid, double* endx, double* endy, double sunLat, double sunLong, double originalSlope, double originalIntercept){
    //Determine which grid edge the line running between the sun and current point
    //intersects first by calculating corners (starting at lower left and moving
    //clockwise
    double cornerOneLat = grid->latitude;
    double cornerOneLong = grid->longitude;
    
    double cornerTwoLat = convertIToLat(0, grid);
    double cornerTwoLong = grid->longitude;
    
    double cornerThreeLat = cornerTwoLat;
    double cornerThreeLong = convertJToLong(grid->cols, grid);
    
    double cornerFourLat = grid->latitude;
    double cornerFourLong = cornerThreeLong;
    
    //Horizontal lines (one is bottom, two is top of grid), slope of zero
    double slopeOne = 0;
    double interceptOne = grid->latitude;
    double slopeThree = 0;
    double interceptThree = cornerTwoLat;
    
    //Vertical lines (two is left, four is right), slope of biggest float
    double slopeTwo = FLT_MAX;
    double interceptTwo = cornerTwoLat - slopeTwo*cornerTwoLong;
    double slopeFour = FLT_MAX;
    double interceptFour = cornerFourLat - slopeFour*cornerFourLong;
    
    //TODO: These bits of code could be combined into function, have to account for changes though...
    //Determine if intersection with bottom grid border is correct
    double xOne = calcXIntersection(interceptOne, originalIntercept, originalSlope, slopeOne);
    //double yOne = calcYIntersection(originalSlope, originalIntercept, xOne);
    //In case slope is vertical, intersection with bottom line is bottom line lat, sun long
    if (originalSlope == FLT_MAX && sunLat < *endy){
        *endy = cornerOneLat;
        *endx = sunLong;
        return 1;
    }
    else if (originalSlope < 0 && *endy >= cornerOneLat && *endx < xOne && isOnLine(xOne, cornerOneLong, cornerFourLong) == 1){
        *endy = cornerOneLat;
        *endx = xOne;
        return 1;
    }
    else if (originalSlope >= 0 && *endy >= cornerOneLat && *endx >= xOne && isOnLine(xOne, cornerOneLong, cornerFourLong) == 1){
        *endy = cornerOneLat;
        *endx = xOne;
        return 1;
    }
    
    //determine if intersection with left grid border is correct
    double xTwo = calcXIntersection(interceptTwo, originalIntercept, originalSlope, slopeTwo);
    double yTwo = calcYIntersection(originalSlope, originalIntercept, xTwo);
    //In case slope is horizontal, intersection with left line is sun lat, left line long
    if (originalSlope == 0 && sunLong < *endx){
        *endy = sunLat;
        *endx = cornerTwoLong;
        return 0;
    }
    else if (originalSlope < 0 && *endy < yTwo && *endx >= cornerTwoLong && isOnLine(yTwo, cornerOneLat, cornerTwoLat) == 1){
        *endy = yTwo;
        *endx = cornerTwoLong;
        return 0;
    }
    else if (originalSlope >= 0 && *endy >= yTwo && *endx >= cornerTwoLong && isOnLine(yTwo, cornerOneLat, cornerTwoLat) == 1){
        *endy = yTwo;
        *endx = cornerTwoLong;
        return 0;
    }
    
    //determine if intersection with top grid border is correct
    double xThree = calcXIntersection(interceptThree, originalIntercept, originalSlope, slopeThree);
    //double yThree = calcYIntersection(originalSlope, originalIntercept, xThree);
    //In case slope is vertical, intersection with top line is top line lat, sun long
    if (originalSlope == 0 && sunLong < *endx){
        *endy = cornerThreeLat;
        *endx = sunLong;
        return 1;
    }
    else if (originalSlope < 0 && *endy <= cornerThreeLat && *endx >= xThree && isOnLine(xThree, cornerTwoLong, cornerThreeLong) == 1){
        *endy = cornerThreeLat;
        *endx = xThree;
        return 1;
    }
    else if (originalSlope >= 0 && *endy <= cornerThreeLat && *endx < xThree && isOnLine(xThree, cornerTwoLong, cornerThreeLong) == 1){
        *endy = cornerThreeLat;
        *endx = xThree;
        return 1;
    }
    
    //determine if intersection with right grid border is correct
    double xFour = calcXIntersection(interceptFour, originalIntercept, originalSlope, slopeFour);
    double yFour = calcYIntersection(originalSlope, originalIntercept, xFour);
    //In case slope is horizontal, intersection with right line is sun lat, right line long
    if (originalSlope == 0 && sunLong < *endx){
        *endy = sunLat;
        *endx = cornerFourLong;
        return 0;
    }
    else if (originalSlope < 0 && *endy >= yFour && *endx < cornerFourLong && isOnLine(yFour, cornerFourLat, cornerThreeLat) == 1){
        *endy = yFour;
        *endx = cornerFourLong;
        return 0;
    }
    else if (originalSlope >= 0 && *endy < yFour && *endx < cornerFourLong && isOnLine(yFour, cornerFourLat, cornerThreeLat) == 1){
        *endy = yFour;
        *endx = cornerFourLong;
        return 0;
    }
    return 0;
}

/**
 *Find x coordinate of intersection between two lines
 */

static inline double calcXIntersection(double interceptB, double interceptA, double slopeA, double slopeB){
    return (interceptB-interceptA)/(slopeA-slopeB);
}

/**
 *Find y coordinate of intersection between two lines
 */

static inline double calcYIntersection(double slope, double intercept, double x){
    return (slope*x)+intercept;
}

/**
 *Convert latitude to i (y)
 */
static inline double convertLatToI(double latitude, Grid* grid){
    return (grid->rows - 1) - ((latitude - grid->latitude)/(grid->cellsize));
}

/**
 *Convert longitude to j (x)
 */
static inline double convertLongToJ(double longitude, Grid* grid){
    return (longitude - grid->longitude)/grid->cellsize;
}

/**
 *Convert i (y) to latitude
 */
static inline double convertIToLat(double i, Grid* grid){
    return grid->latitude + grid->cellsize*(grid->rows-i-1);
}

/**
 *Convert j (x) to longitude
 */
static inline double convertJToLong(double j, Grid* grid){
    return grid->longitude + grid->cellsize*j;
}

/**
 *Computes whether (i,j) represented by currentLat, currentLong, is visible from the point directly
 *under the sun. Throughout this method, it is pivotal to remember that y is row (i) and x is col (j)
 */

int pointVisibleFromSun(Grid* elevGrid, Grid* energyGrid, double currentLat, double currentLong, int i, int j, double sunLat, double sunLong, double dayNum, double localTime, double turbidity, double timeStep){
    //use visible as a boolean variable, 1 represents visible, 0 not visible
    int visible = 1;
    double slope;
    double intercept;
    
    float newHeight = elevGrid->data[i][j];
    double energy = 0;
    
    //Use these points (start and end) to make sure lines always run from left to right
    //Later, we will use these same points to make sure lines run from bottom to top
    double startx;
    double starty;
    double endx;
    double endy;
    
    //Make sure lines are always running from left to right
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
    slope = calculateSlope(startx, starty, endx, endy);
    
    //intercept is y - slope*x = b
    if (slope == FLT_MAX){
        intercept = FLT_MAX;
    }
    else if (endx == sunLong) {
        intercept = starty - (slope*startx);
    }
    else {
        intercept = endy - (slope*endx);
    }
    
    //We want to replace sunLat and sunLong with a newly calculated point that intersects at the edge of the grid
    double newX, newY;
    if (endx == sunLong && endy == sunLat){
        newX = startx;
        newY = starty;
    }
    else {
        newX = endx;
        newY = endy;
    }
    
    int isHorizontal = findNewEndPoint(elevGrid, &newX, &newY, sunLat, sunLong, slope, intercept);
    
    if (endx == sunLong && endy == sunLat){
        endx = newX;
        endy = newY;
    }
    else {
        startx = newX;
        starty = newY;
    }
    
    //xAxis is the variable that is incremented by 1 unit each time
    //It represents the intersection point's x (longitude) value
    double xAxis = startx;
    
    //Account for intersection point not falling on longitude line in grid
    if (isHorizontal == 1){
        double jForX = convertLongToJ(xAxis, elevGrid);
        double jCorrected = ceil(jForX);
        xAxis = convertJToLong(jCorrected, elevGrid);
    }
    
    //Calculate the angle of elevation between the new point and the sun
    double solarAngleNew = calcSunAngle(dayNum, localTime, currentLat, newHeight, calcHaversine(sunLong, sunLat, currentLong, currentLat));
    
    //Calculate length of single cell in meters
    double cellLength = calcHaversine(elevGrid->longitude, elevGrid->latitude, elevGrid->longitude+elevGrid->cellsize, elevGrid->latitude);

    xAxis += elevGrid->cellsize;
    
    //printf("X Checks\n");
    
    for (; xAxis < endx; xAxis+=elevGrid->cellsize){
        //If the difference is not significant enough, break out of loop
        if (fabs(xAxis-endx) < ERROR){
            break;
        }
        //intersectY represents the intersection point's y (latitude) values
        float intersectYLat;
        if (slope == 0){
            intersectYLat = sunLat;
        }
        else {
            intersectYLat = (slope*xAxis) + intercept;
        }
        
        double intersectY = convertLatToI(intersectYLat, elevGrid);
        int intersectX = (int) convertLongToJ(xAxis, elevGrid);

        //h1y represents the y (latitude) value of the point below the intersection point
        int h1y = ceil(intersectY);
        //h2y represents the y (latitude) value of the point above the intersection point
        int h2y = floor(intersectY);
        //elevH1 is the elevation of the h1 point
        float elevH1 = elevGrid->data[h1y][intersectX];
        //elevH2 is the elevation of the h2 point
        float elevH2 = elevGrid->data[h2y][intersectX];
        
        //elevAngle is the angle of elevation between the h1 and h2 points
        //float elevAngle;
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
        //elevAngle = atan((elevDifference)/1);//elevGrid->cellsize
        
        //pY is the relevant decimal of the intersect point's y value
        double pY = intersectY-floor(intersectY);
        //intersectHeight is the elevation of the intersection point
        //float intersectHeight = lowerHeight + (pY*tan(elevAngle));
        double intersectHeight = lowerHeight + elevDifference * pY;
        
        //Calculate the angle of elevation between the interpolated point and the sun
        double solarAngleIntersection = calcSunAngle(dayNum, localTime, intersectYLat, intersectHeight, calcHaversine(sunLong, sunLat, xAxis, intersectYLat));
        //Calculate the angle of elevation between the new point and the sun
        //double solarAngleNew = calcSunAngle(dayNum, localTime, convertIToLat(i, elevGrid), newHeight, calcHaversine(sunLong, sunLat, currentLong, currentLat));
        
        //Compare the angle of elevations of the sun and the new point and the sun and the interpolated point
        //If the angle of elevation between the sun and the intersect point is less
        //than the angle of elevation between the sun and the new point, the view path
        //is obstructed
        if (solarAngleIntersection < solarAngleNew){
            energyGrid->data[i][j] += calcGigaJoulesOverTimeStep(calcGlobalIrradiance(newHeight, turbidity, dayNum, solarAngleNew, 0), cellLength*cellLength, convertTimestepToSeconds(timeStep));
            return 0;
        }
    }
    
    //Make sure lines are always running from bottom to top
    //The point (i,j) represented by (currentLat, currentLong) is farther up than (sunLat, sunLong),
    //make (i,j) represented by (currentLat, currentLong) the end point
    if (sunLat < currentLat){
        startx = sunLong;
        starty = sunLat;
        endx = currentLong;
        endy = currentLat;
    }
    //The point (i,j) represented by (currentLat, currentLong) is farther down than (sunLat, sunLong),
    //make (i,j) represented by (currentLat, currentLong) the start point
    else {
        startx = currentLong;
        starty = currentLat;
        endx = sunLong; 
        endy = sunLat;
    }
    
    //recalculate the slope and intercept (points may have changed)
    slope = calculateSlope(startx, starty, endx, endy);
    //Account for case of infinite slope
    if (slope == FLT_MAX){
        intercept = FLT_MAX;
    }
    else if (endx == sunLong) {
        intercept = starty - (slope*startx);
    }
    else {
        intercept = endy - (slope*endx);
    }
    
    //We want to replace sunLat and sunLong with a newly calculated point that intersects at the edge of the grid
    //double newX, newY;
    if (endx == sunLong && endy == sunLat){
        newX = startx;
        newY = starty;
    }
    else {
        newX = endx;
        newY = endy;
    }
    
    //Update isHorizontal (0 for vertical, 1 for horizontal)
    isHorizontal = findNewEndPoint(elevGrid, &newX, &newY, sunLat, sunLong, slope, intercept);
    
    if (endx == sunLong && endy == sunLat){
        endx = newX;
        endy = newY;
    }
    else {
        startx = newX;
        starty = newY;
    }
    
    //Modify the yAxis so that the latitude falls on discrete latitudes that
    //correspond to grid latitude values
    double yAxis = starty;
    if (isHorizontal == 0){
        double iForY = convertLatToI(yAxis, elevGrid);
        double iCorrected = floor(iForY);
        yAxis = convertIToLat(iCorrected, elevGrid);
    }
    
    //yAxis += elevGrid->cellsize;
    
    //Interpolate for y (latitude)
    //The process below is similar to that above, except the y (latitude) is incremented
    //and the intersection point's x value (longitude) is calculated
    
    for (; yAxis < endy; yAxis+=elevGrid->cellsize){
        //If the difference is not significant enough, break out of loop
        if (fabs(yAxis-endy) < ERROR){
            break;
        }
        
        //intersectX represents the intersection point's x (longitude) value
        float intersectXLong;
        if (slope == FLT_MAX){
            intersectXLong = sunLong;
        }
        else {
            intersectXLong = (yAxis-intercept)/slope;
        }

        double intersectX = convertLongToJ(intersectXLong, elevGrid);
        int intersectY = (int) convertLatToI(yAxis, elevGrid);
        
        //h1x represents the x (longitude) value of the point right of the intersection point
        int h1x = ceil(intersectX);
        //h2x represents the x (longitude) value of the point left of the intersection point
        int h2x = floor(intersectX);
        //elevH1 is the elevation of the h1 point
        float elevH1 = elevGrid->data[intersectY][h1x];
        //elevH2 is the elevation of the h2 point
        float elevH2 = elevGrid->data[intersectY][h2x];
        
        //elevAngle is the angle of elevation between the h1 and h2 points
        //float elevAngle;
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
        //elevAngle = atan((elevDifference)/1);

        //pX is the relevant decimal of the intersect point's x value
        double pX = intersectX-floor(intersectX);
        //intersectHeight is the elevation of the intersection point
        //float intersectHeight = lowerHeight + (pX*tan(elevAngle));
        double intersectHeight = lowerHeight + elevDifference * pX;
        
        //Calculate the angle of elevation between the interpolated point and the sun
        double solarAngleIntersection = calcSunAngle(dayNum, localTime, yAxis, intersectHeight, calcHaversine(sunLong, sunLat, intersectXLong, yAxis));
        //Calculate the angle of elevation between the new point and the sun
        //double solarAngleNew = calcSunAngle(dayNum, localTime, convertIToLat(i, elevGrid), newHeight, calcHaversine(sunLong, sunLat, currentLong, currentLat));
        
        //Compare the angle of elevations of the sun and the new point and the sun and the interpolated point
        //If the angle of elevation between the sun and the intersect point is less
        //than the angle of elevation between the sun and the new point, the view path
        //is obstructed
        if (solarAngleIntersection < solarAngleNew){
            energyGrid->data[i][j] += calcGigaJoulesOverTimeStep(calcGlobalIrradiance(newHeight, turbidity, dayNum, solarAngleNew, 0), cellLength*cellLength, convertTimestepToSeconds(timeStep));
            return 0;
        }
    }
    //If energy has not been calculated yet (is visible or no interpolation), calculate it
    if (energy == 0){
        energy = calcGlobalIrradiance(newHeight, turbidity, dayNum, solarAngleNew, 1);
    }
    
    //Calculate the energy in GigaJoules over the timestep (assume irradiance is constant over timestep)
    double energyGigaJoules = calcGigaJoulesOverTimeStep(energy, cellLength*cellLength, convertTimestepToSeconds(timeStep));
    //Add to the energy grid
    energyGrid->data[i][j] += energyGigaJoules;
    
    return visible;
}

/**
 *Compute the viewshed
 */
void computeViewshed(Grid* elevGrid, Grid* energyGrid, double startTime, double endTime, double timeStep, double dayNum, double timeZone, double turbidity, int numThreads){
    for (; startTime < endTime; startTime += timeStep){
        //Initialize a new viewshed grid for each timestep
        Grid* viewshedGrid;
        viewshedGrid = gridInit(elevGrid);
        
        //calculate lat and long of position directly under sun
        double sunLat = calcSunLat(dayNum);
        double sunLong = calcSunLong(startTime, timeZone);
        
        pthread_t thread[numThreads];
        struct ThreadData data[elevGrid->rows];
        
        int i = 0;
        for (i = 0; i < elevGrid->rows; i++){
            data[i].elevGrid = elevGrid;
            data[i].viewshedGrid = viewshedGrid;
            data[i].energyGrid = energyGrid;
            data[i].startTime = startTime;
            data[i].endTime = endTime;
            data[i].timeStep = timeStep;
            data[i].dayNum = dayNum;
            data[i].timeZone = timeZone;
            data[i].turbidity = turbidity;
            data[i].sunLat = sunLat;
            data[i].sunLong = sunLong;
            data[i].i = i;
            viewshedLoops(&data[i]);
        }
        
        /*
        for (i=0; i<elevGrid->rows; i++) {
            pthread_create(&thread[i], NULL, viewshedLoops, &data[i]);
        }
        
        for (i=0; i<elevGrid->rows; i++) {
            pthread_join(thread[i], NULL);
        }
        */
        
        //Create unique name for new viewshed and write to that file
        char buf[100];
        char nameDirectory[] = "./VIEWSHEDS/";
        mkdir("./VIEWSHEDS/", 0777);
        char nameBegin[] = "viewshed";
        char nameEnd[] = ".asc";
        sprintf(buf, "%s%s%f%s", nameDirectory, nameBegin, startTime, nameEnd);
        writeFile(buf, viewshedGrid);
    }
}

void* viewshedLoops(struct ThreadData* data){
    int j;
    if (data->i%10 == 0){
        printf("%d\n", data->i);
    }
    //Calculate lat of current cell from bottom left corner of grid
    double currentLat = convertIToLat(data->i, data->elevGrid);
    for (j = 0; j < data->elevGrid->cols; j++){
        //Calculate long of current cell from bottom left corner of grid
        double currentLong = convertJToLong(j, data->elevGrid);
        
        if (data->viewshedGrid->data[data->i][j] == data->viewshedGrid->noDataValue){
            data->viewshedGrid->data[data->i][j] = 0;
            continue;
        }
        //see if visible (if point directly under sun), if yes set appropriate cell to 1
        if (currentLat == data->sunLat && currentLong == data->sunLong){
            data->viewshedGrid->data[data->i][j] = 1;
            continue;
        }
        
        //Finally, determine if point is visible via interpolation
        int visible = pointVisibleFromSun(data->elevGrid, data->energyGrid, currentLat, currentLong, data->i, j, data->sunLat, data->sunLong, data->dayNum, data->startTime, data->turbidity, data->timeStep);
        
        if (visible == 1){
            data->viewshedGrid->data[data->i][j] = 1;
        }
        else {
            data->viewshedGrid->data[data->i][j] = 0;
        }
    }
    return NULL;
}

