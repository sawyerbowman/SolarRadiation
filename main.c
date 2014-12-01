//main.c

#include "viewshed.h"
#include "solarPosition.h"

int main(int argc, char** args){
    
    double test = calcSunAngle(336, 12, 45.52);
    printf("%f\n", test);
    
    /*
    char *elevfname, *viewfname;
    
    if (argc != 5){
        printf("Enter a filename to read into the program, a filename to store the output, and"
               "two integers representing a point.\n");
        return 0;
    }
    
    elevfname = args[1];
    viewfname = args[2];
    int vpi = atoi(args[3]);
    int vpj = atoi(args[4]);
    printf("%s\n", elevfname);
    printf("%s\n", viewfname);
    printf("%d\n", vpi);
    printf("%d\n", vpj);
    
    //Initialize the elevGrid from the file
    Grid* elevGrid;
    elevGrid = readFile(elevfname);
    
    //Initialize the viewshedGrid from the elevGrid
    Grid* viewshedGrid;
    viewshedGrid = viewshedGridInit(elevGrid);
    
    //printHeader(viewshedGrid);
    //printValues(viewshedGrid);
    
    computeViewshed(elevGrid, viewshedGrid, vpi, vpj);
    
    printHeader(viewshedGrid);
    printValues(viewshedGrid);
    
    //Save the viewshed grid to the specified file
    writeFile(viewfname, viewshedGrid);
     */
}