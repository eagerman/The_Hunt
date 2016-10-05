// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "DracView.h"
#include "Places.h"
#include "time.h"

#define NUM_HUNTERS 4
#define TRAIL_SIZE 6

char *firstMove (DracView gameState); 
void allLandLocs (int whereToGo[]);
void getHuntLoc (DracView dv, int huntArray[]); 
void avoidHunterLoc (DracView dv, int whereToGo[], int hunterLocation[], 
                     int *numLocPtr); 
int hideInTrail (DracView dv);
int dbInTrail (DracView dv); 
char *otherMove(DracView dv);
int hideInTrail(DracView dv); 
int dbInTrail(DracView dv);
int checkInTrail(LocationID newLocation, LocationID trail[TRAIL_SIZE]);
int getDB (int inTrail);
LocationID TPorSea (int numWhereCanISea, int whereCanISea[]);

void decideDraculaMove(DracView gameState)
{ 
    Round roundNum = giveMeTheRound(gameState); //DracView.h
    
	char *dracMove;

    if (roundNum == 0) {
        dracMove = firstMove(gameState);
    } else {
        // otherMove() strategy: choose random move 
        // we should do a BFS when we have more time 
        dracMove = otherMove(gameState); 
    }
	registerBestPlay(dracMove, "WASSUP WASSUP WASSUP WASSUP WASSUP");
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
    int huntLoc[NUM_HUNTERS];
    getHuntLoc (gameState, huntLoc); 

    // make loc of each hunters && loc each hunter can go == 0  
    avoidHunterLoc (gameState, whereToGo, huntLoc, numLocPtr); 

    // any other places Drac cannot go in the first turn???
	// choosing a random first move 
    time_t t; 
    srand ((unsigned) time(&t)); 
    int random = rand() % 72; // we used 72 b/c of 71 locations 
    
	while (whereToGo[random] == 0) {
        random = rand() % 72;
    }

    // if whereToGo[rand] == 1 then make that the new location
    int newLoc = random;
    char *firstMove = idToAbrrev(newLoc);  
    
	free(huntLoc); 

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
void getHuntLoc (DracView dv, int huntArray[]) { 
    for (int i = 0; i < NUM_HUNTERS; i++) 
        huntArray[i] = whereIs (dv, i); 
} 

// pre: takes an array int of where to go
// post: removes places where hunters are and where they can go by making == 0
void avoidHunterLoc (DracView dv, int whereToGo[], int hunterLocation[], 
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
char *otherMove(DracView dv) {
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
    // OR just select a random move from the array if no time. 
    if (*numWhereCanILand > 1) {
        int inTrail = -1; //this tells us if in trail
        for (int i = 0; i < *numWhereCanILand; i++) {
            inTrail = checkInTrail(whereCanILand[i], trail); 
            if (inTrail >= 0) {
                if ((whereCanILand[i] == myLoc) && (hide == FALSE) 
                    && (myLocType != SEA)) { 
                    // no hide in trail and not at sea then you can hide 
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
        } // now our LAND array has illegal and legal moves 

        // We are now creating an array of ONLY legal moves 
        // Counting legal possible moves
        int ctr = 0; 
        for (int j = 0; j < *numWhereCanILand; j++) {
            if (whereCanILand[ctr] != -1) ctr++; 
            j++; 
        } 
        // create array that stores legal moves
        // do we have to do [ctr+1] ??????????
        LocationID legalMoves[ctr]; int k = 0; 
        for (j = 0; j < *numWhereCanILand; j++) {
            if (whereCanILand[j] != -1) {
                legalMoves[k] = whereCanILand[j];  
                k++; 
            } 
            j++; 
        }
        // select random move (because cbb breadth first search) 
        int random = rand() % ctr; 
        char* otherMove = idToAbbrev(legalMoves[random]); 
        free(whereCanILand);
        free(whereCanISea); 

        return otherMove;

    } else { // case: only ONE or ZERO land location 
        LocationID newLoc; 
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
        
        // We must now check this location 
        // whether it is inTrail and whether we can HIDE or DB
        // if we can HIDE or DB we should. 
        inTrail = checkInTrail(newLoc, trail);
        if (inTrail = -1) { // no need to do anything
        } else if (inTrail >= 0) { 
            if (inTrail == 0) { // if newLoc is first in 
                if (hide == FALSE) {newLoc = HIDE;} 
                else if (DB == FALSE) {newLoc = {DOUBLE_BACK_1;} //Not sure 
                else newLoc = {TPorSea(*numWhereCanISea, whereCanISea);} 
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


LocationID TPorSea (int numWhereCanISea, int whereCanISea[]) {
    int newLoc; 
    if (numWhereCanISea == 0) {
        // if no sea loc then only TP 
        newLoc = TELEPORT; 
    } else {
        // initialise random num generator
        // and choose random sea loc 
        time_t t; 
        srand((unsigned) time(&t)); 
        i = rand() % numWhereCanISea; 
        newLoc = whereCanISea[i];
    }
}

// converts ID (number) to abbreviation which are two characters
char *idToAbbrev (LocationID id) {
	chart *abbrev;
	
	//check struct in Places.c and Places.h for clarification
	if ((id >= 0) && (id <= 70)) {
		abbrev = places[id]->abbrev; 
	} else if (id == 100 {
		abbrev = "C?"; //CITY_UNKNOWN 
	} else if (id == 101) {
		abbrev = "S?"; //SEA_UNKNOWN
	} else if (id == 102) {
		abbrev = "HI"; 
	} else if (id == 103) {
		abbrev = "D1"; 
	} else if (id == 104) {
		abbrev = "D2"; 
	} else if (id == 105) {
		abbrev = "D3"; 
	} else if (id == 106) {
		abbrev = "D4";
	} else if (id == 107) {
		abbrev = "D5";
	} else if (id == 108) {
		abbrev = "TP"; 
	} else { //something went wrong
		printf("Something fked up\n"); 
		name = "?";
	}
	return name; 
}

















