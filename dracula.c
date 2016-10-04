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
    int newLoc = rand;
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
    LocationID myLoc = whereIs(dv, PLAYER_DRACULA); 
    int myLocType = idToType(myLoc); 
 
    // see whether hide or DB is in trail TRUE or FALSE 
    int hide = hideInTrail(dv);
    int DB = dbInTrail(dv);

    // initialising Drac's trail by putting into an array 
    LocationID trail[TRAIL_SIZE]; 
    giveMeTheTrail(dv, PLAYER_DRACULA, trail); 

    // remove moves that are not possible in whereCanILand 
    // then do a BFS to find furthest location from the hunters 
    if (*numWhereCanILand > 1) {
        int inTrail = -1; //this tells us if in trail
        for (int i = 0; i < *numWhereCanILand; i++) {
            inTrail = checkInTrail(whereCanILand[i], trail); 
            if (inTrail >= 0) {
                if ((whereCanILand[i] == myLoc) && (hide == FALSE) 
                    && (myLocType != SEA)) { 
                    // no hide and not at sea then you can hide 
                    whereCanILand[i] = HIDE; 
                } else if ((whereCanILand[i] == myLoc) && (hide == TRUE)) { 
                    // if hide true then cannot hide 
                    whereCanILand[i] = -1; //cannot HIDE 
                } else if (whereCanILand[i] = ST_JOSEPH_AND_ST_MARYS) {
                    whereCanILand[i] = -1; 
                } else if ((DB == FALSE) && myLocType != SEA) { 
                    whereCanILand[i] = getDB(inTrail); 
                } else if (DB == TRUE) { // cannot DB
                    whereCanILand[i] = -1; 
                }
            } else { /* not in trail*/ }
        } // now our LAND array has all legal moves 
        // write function to copy possible legal moves to new array
    } else {  
        LocationID newLoc; 
        // if only ONE land location
        if ((*numWhereCanILand == 0) && (myLocType == SEA)) {
            // no land loc and at sea therefore only TP or Sea available
            newLoc = TPorSea(*numWhereCanISea, whereCanISea);   
        } else if (*numWhereCanILand == 1){
            // only one land loc then choose that one
            newLoc = whereCanILand[0];
            if (newLoc == ST_JOSEPH_AND_ST_MARYS) newLoc = TELEPORT; 
        } else {
            newLoc = TPorSea(*numWhereCanISea, whereCanISea); 
        }
        
        // after we make the only move our next Location we must check
        // whether it is inTrail and whether we can HIDE or DB
        // if we can HIDE or DB we should. 
        inTrail = checkInTrail(newLoc, trail);
        if (inTrail = -1) { // no need to do anything
        } else if (inTrail >= 0) { 
            if (inTrail == 0) { // if newLoc is first in 
                if (hide == FALSE) newLoc = HIDE; 
                else if (DB == FALSE) newLoc = DOUBLE_BACK_1; 
                else newLoc = TPorSea(*numWhereCanISea, whereCanISea); 
            } else if (DB == FALSE) {newLoc = getDB(inTrail);}   
        }
    }
    char *otherMove = idToAbbrev(newLoc); 
    free(whereCanILand); 
    free(whereCanISea); 

    return otherMove;  
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


// checks where LocationID is in trial, if not return -1 
int checkInTrail(LocationID newLocation, LocationID trail[TRAIL_SIZE]){
    int inTrail = -1; // by default not in trial 
    for (int i = 0; i < TRAIL_SIZE; i++) {
        if ((trail[i] == newLocation) && (trail[i] != UNKNOWN_LOCATION)){
            inTrail = i; 
            break;  
    }
    return inTrail;  
} 


// gets double back in trail 
int getDB (int inTrail) {
    int DB; 
    switch (inTrail) {
        case '1': 
            whereCanILand[i] = DOUBLE_BACK_1;
            break;
        case '2': 
            whereCanILand[i] = DOUBLE_BACK_2;
            break;
        case '3': 
            whereCanILand[i] = DOUBLE_BACK_3;
            break;
        case '4': 
            whereCanILand[i] = DOUBLE_BACK_4;
            break;
        case '5': 
            whereCanILand[i] = DOUBLE_BACK_5;
            break;
    }
    return DB;
}
