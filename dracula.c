// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "DracView.h"
#include "Places,h"

#define NUM_HUNTERS 4
#define TRAIL_SIZE 6

char *firstMove (DracView gameState); 
void allLandLocs (int whereToGo[]);
void getHuntLoc (DracView dv, int *huntArray); 
void avoidHunterLoc (DracView dv, int whereToGo[], int *hunterLocation, 
                     int *numLocPtr); 
int hideInTrail (DracView dv);
int dbInTrail (DracView dv); 

void decideDraculaMove(DracView gameState)
{ 
    Round roundNum = giveMeTheRound(gameState); //DracView.h
    
    if (roundNum == 0) {
        char *DracMove = firstMove(gameState);
    else {
        char *otherDracMove = otherMove(gameState); 
        // BFS search for furthest location away from the nearest hunter  
    }
	registerBestPlay(DracMove, "WASSUP WASSUP WASSUP WASSUP WASSUP");
}


char *firstMove (DracView gameState) {
    
    int numLoc = 0; 
    int *numLocPtr = &numLoc; 
    // create an array for locations Drac can go
    int whereToGo[NUM_MAP_LOCATIONS];
    //put all land locations into whereToGo 
    allLandLocs (whereToGo);
    // he cannot go to CASTLE_DRACULA or ST_MARY hospital
    whereToGo[CASTLE_DRACULA] = 0;
    whereToGo[ST_JOSEPH_AND_ST_MARYS] = 0;
    // store the current location of each hunter in an array 
    int *huntLoc[NUM_HUNTERS];
    getHuntLoc (gameState, huntLoc); 
    // make loc of each hunters && loc each hunter can go == 0  
    avoidHunterLoc (gameState, whereToGo, *huntLoc, numLocPtr); 
    // any other places Drac cannot go in the first turn??? 
    int rand = rand() % 71; // rand() % NUM_MAP_LOCATIONS 
    while (whereToGo[rand] == 0) {
        rand = rand() % 71;
    }
    // if whereToGo[rand] == 1 then make that the new location
    int newLocation = rand;
    char *firstMove = idToAbrrev(newLocation);  
    free(hunterLoc); 
    //I don't think we need to free numLov etc b/c we might use them again
    return firstMove;
}


// pre: take int array 
// post: puts all land locations that Drac can go b/c he can only 
//       go on land for the first move
void allLandLocs (int whereToGo[]) {
    int i; 
    for (i = 0; i < NUM_MAP_LOCATIONS; i++) { 
        if (idToType(i) == LAND) {
            whereToGo[i] = 1; 
        } else {
            whereToGo[i] = 0; 
        }
    }
}


// pre: takes an int array with space to store loc of each hunter
// post: finds their location and puts it into the array
void getHuntLoc (DracView dv, int *huntArray) { 
    for (int i = 0; i < NUM_HUNTERS; i++) 
        huntArray[i] = whereIs (dv, i); 
} 

// pre: takes an array int of where to go
// post: removes places where hunters are and where they can go by making == 0
void avoidHunterLoc (DracView dv, int whereToGo[], int *hunterLocation, 
                  int *numLocPtr) {
    LocationID *whereCanHunterGo; 
    for (int i = 0; i < NUM_HUNTERS; i++) {
        // make location of each hunter not an option
        whereToGo[huntLoc[i]] = 0; 
        // create an a LocationID * array with loc where hunt can go
        whereCanHunterGo = whereCanTheyGo (dv, numLocPtr, i, TRUE, TRUE, TRUE);
        // delete data in whereToGo that corresponds to where a hunter can go
        for (int j = 0; j < *numLocPtr; j++) {
            whereToGo[whereCanHunterGo[j]] = 0;
        } 
    }
}

// pre: takes the dv and the location of drac 
// post: finds best next move for drac by returning a string 
char *otherMove(DracView dv, LocationID dracLoc) {
    // initialise pointer to store the number of locations Drac can go
    int x, y; 
    int *numWhereCanILand, *numWhereCanISea;
    numWhereCanILand = &x; numWhereCanISea = &y; //they pt data in x & y 
    // store these locations in an array for Land and sea locations
    LocationID *whereCanILand = whereCanIgo(dv, numWhereCanILand, TRUE, FALSE);          
    LocationID *whereCanISea= whereCanIgo(dv, numWhereCanISea, FALSE, TRUE);
 
    // see whether hide or DB is in trail TRUE or FALSE 
    int hide = hideInTrail(dv);
    int DB = dbInTrail(dv);

    // Do something w/ above info to find best next move  
}

int hideInTrail(DracView dv) {
    int result = FALSE; 
    // initialise and fill array with trail locations 
    int trail[TRAIL_SIZE];
    giveMeTheTrail(dv, PLAYER_DRACULA, trail); 
    // go through the array and see if there is HIDE;
    for (int i = 0, i < TRAIL_SIZE; i++) {
        if (trail[i] == HIDE) {
            result = TRUE; 
        }
    }
    return result; 
}

int dbInTrail(DracView dv) {
    int result = FALSE; 
    // initialise and fill array with trail locations 
    int trail[TRAIL_SIZE];
    giveMeTheTrail(dv, PLAYER_DRACULA, trail); 
    // go through the array and see if there is DB;
    for (int i = 0, i < TRAIL_SIZE; i++) {
        if ((trail[i] >= DOUBLE_BACK_1) && 
            (trail[i] <= DOUBLE_BACK_5)) {
            result = TRUE; 
        } 
}
 
