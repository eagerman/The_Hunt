// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

/*
Ending the round at DC will gain me 10lifePts, but hunters will know that
i can only move by road or sea(i'll loose 2 lifePts)
which is better for me placing vampires or traps?
    hunters will loose 2 lifePts when encounter a trap
    but no lifePts when encounter and kill my vampires
    but my vampires reduce score by 13 when mature
    can i place both?

*/

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
int dbInTrail(DracView dv); 
char *otherMove(DracView dv);
int hideInTrail(DracView dv); 
int dbInTrail(DracView dv);
int checkInTrail(LocationID newLocation, LocationID trail[TRAIL_SIZE]);
int getDB (int trailIndex, LocationID whereCanILand);
LocationID TPorSea (int numWhereCanISea, int whereCanISea[]);

void decideDraculaMove(DracView gameState)	
{ 
    Round roundNum = giveMeTheRound(gameState); //DracView.h
	char *dracMove;

    if (roundNum == 0) {
        dracMove = firstMove(gameState);
    } else { 		
		printf("This is round %d and we are using otherMv()\n", roundNum);		
        // otherMove() strategy: choose random move 
        // we should do a BFS when we have more time 
        //dracMove = otherMove(gameState); 
        dracMove = firstMove(gameState); 
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
    char *firstMoveStr; 
    firstMoveStr = idToAbbrev(newLoc);  
    
	//free(huntLoc); 

    return firstMoveStr;
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
    int i;
    for (i = 0; i < NUM_HUNTERS; i++) 
        huntArray[i] = whereIs (dv, i); 
} 

// pre: takes an array int of where to go
// post: removes places where hunters are and where they can go by making == 0
void avoidHunterLoc (DracView dv, int whereToGo[], int huntLocation[], 
                     int *numLocPtr) {
    LocationID *whereCanHunterGo;
    int i; 
    for (i = 0; i < NUM_HUNTERS; i++) {
        // make location of each hunter not an option
        whereToGo[huntLocation[i]] = 0; 
        // create an a LocationID * array with loc where hunt can go
        whereCanHunterGo = whereCanTheyGo (dv, numLocPtr, i, TRUE, TRUE, TRUE);
        // delete data in whereToGo that corresponds to where a hunter can go
        int j;
        for (j = 0; j < *numLocPtr; j++) {
            whereToGo[whereCanHunterGo[j]] = 0;
        } 
    }
}

// pre: takes the dv and the location of drac 
// post: finds best next move for drac by returning a string 
char *otherMove(DracView dv) {
	char *otherMoveStr;    
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
        int trailIndex = -1; //this tells us if in trail
        int i;
        for (i = 0; i < *numWhereCanILand; i++) {
            trailIndex = checkInTrail(whereCanILand[i], trail); 

            if (trailIndex >= 0) {
                if ((whereCanILand[i] == myLoc) && (hide == FALSE) 
                    && (myLocType != SEA)) { 
					printf ("Current Location = %s\n", idToAbbrev(whereCanILand[i])); 
                    // no hide in trail and not at sea then you can hide 
                    whereCanILand[i] = HIDE; 
                } else if ((whereCanILand[i] == myLoc) && (hide == TRUE)) { 
                    // if hide true then cannot hide 
                    whereCanILand[i] = -1; //cannot HIDE 
					printf ("Current Location = %s\n", idToAbbrev(whereCanILand[i])); 
                } else if (whereCanILand[i] == ST_JOSEPH_AND_ST_MARYS) {
					printf ("Current Location = %s\n", idToAbbrev(whereCanILand[i])); 
                    whereCanILand[i] = -1; 
                } else if ((DB == FALSE) && myLocType != SEA) { 
					printf ("Current Location = %s\n", idToAbbrev(whereCanILand[i])); 
                    getDB(trailIndex, whereCanILand[i]); 
                } else if (DB == TRUE) { // cannot DB
					printf ("Current Location = %s\n", idToAbbrev(whereCanILand[i])); 
                    whereCanILand[i] = -1; 
                }
            } else { /* not in trail*/ }
        } // now our LAND array has illegal and legal moves 

        // We are now creating an array of ONLY legal moves 
        // Counting legal possible moves
        int ctr = 0; 
        int j;
        for (j = 0; j < *numWhereCanILand; j++) {
            if (whereCanILand[ctr] != -1) ctr++; 
            j++; 
        } 
        // create array that stores legal moves
        LocationID legalMoves[ctr]; int k = 0; 
        //int j;
        printf("Legal Drac moves: ");
        for (j = 0; j < *numWhereCanILand; j++) {
            if (whereCanILand[j] != -1) {
                legalMoves[k] = whereCanILand[j]; 
				printf("%s ",idToAbbrev(legalMoves[k])); 
                k++; 
            } 
            j++; 
        }
        printf("\n");

        // select random move (because cbb breadth first search) 
		time_t t; 
    	srand ((unsigned) time(&t));         
		int random = rand() % ctr; 
		otherMoveStr = idToAbbrev(legalMoves[random]); 
        printf("moving to (otherMvStr): %s\n", otherMoveStr);
		free(whereCanILand);
		free(whereCanISea); 
		//int huntArray[NUM_HUNTERS];
		//getHuntLoc (dv, huntLoc); 	
		//move(int huntArray); 

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
        // whether it is trailIndex and whether we can HIDE or DB
        // if we can HIDE or DB we should. 
        int trailIndex = checkInTrail(newLoc, trail);
        if (trailIndex == -1) { // no need to do anything
        } else if (trailIndex >= 0) { 
            if (trailIndex == 0) { // if newLoc is first in 
                if (hide == FALSE) {newLoc = HIDE;} 
                //else if (DB == FALSE) {newLoc = DOUBLE_BACK_1;} //Not sure 
                else {newLoc = TPorSea(*numWhereCanISea, whereCanISea);} 
            } else if (DB == FALSE) {getDB(trailIndex, newLoc);}//check   
        }
		otherMoveStr = idToAbbrev(newLoc); 
    	free(whereCanILand); 
    	free(whereCanISea); 

    }

    return otherMoveStr;  
}


int hideInTrail(DracView dv) {
    int result = FALSE; 
    // initialise and fill array with trail locations 
    int trail[TRAIL_SIZE];
    giveMeTheTrail(dv, PLAYER_DRACULA, trail); 
    // go through the array and see if there is HIDE;
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
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
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
        if ((trail[i] >= DOUBLE_BACK_1) && 
            (trail[i] <= DOUBLE_BACK_5)) {
            result = TRUE; 
        } 
	}
	return result;
}


// checks where LocationID is in trial, if not return -1 
int checkInTrail(LocationID newLocation, LocationID trail[TRAIL_SIZE]){
    int trailIndex = -1; // by default not in trial 
    int i;
    for (i = 0; i < TRAIL_SIZE; i++) {
			//printf("intrail = %d\n",trail[i]);
        if ((trail[i] == newLocation) && (trail[i] != UNKNOWN_LOCATION)){
            trailIndex = i; 
			//printf("found %d\n",trailIndex);
            break;  
		}
    }
    return trailIndex;  
} 


// gets double back in trail 
int getDB (int trailIndex, LocationID whereCanILand) {
    switch (trailIndex) {
        case 1: 
            whereCanILand = DOUBLE_BACK_1;
            break;
        case 2: 
            whereCanILand = DOUBLE_BACK_2;
            break;
        case 3: 
            whereCanILand = DOUBLE_BACK_3;
            break;
        case 4: 
            whereCanILand = DOUBLE_BACK_4;
            break;
        case 5: 
            whereCanILand = DOUBLE_BACK_5;
            break;
    }
    return whereCanILand;
}


LocationID TPorSea (int numWhereCanISea, int whereCanISea[]) {
    LocationID newLoc;    
	if (numWhereCanISea == 0) {
        // if no sea loc then only TP 
		newLoc = TELEPORT; 
    } else {
        // initialise r andom num generator
        // and choose random sea loc 
        time_t t; 
        srand((unsigned) time(&t)); 
        int i = rand() % numWhereCanISea; 
		newLoc = whereCanISea[i];
    }
    return newLoc;
}

















