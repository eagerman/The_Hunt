// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Game.h"
#include "HunterView.h"
#include "hunter.h"
#include "Map.h"
#include "Queue.h"
//#include "Queue.h"

#define LIFETHRESHOLD 6
#define DEBUGGING 0

void restHunter(HunterView hv, int num);
void mapSearch(HunterView hv , int roundMod, int player);
void attack(HunterView hv, LocationID dracFoundLoc, int player , int roundMod);
int isValidLoc(int trail);
LocationID *getShortestPath(Map map, LocationID currLoc, LocationID dest, int *length);
int isValueInArray(int val, int *arr, int size);
int isValidTrail(int trail);

//Global Variables
static LocationID dracTrail[TRAIL_SIZE];
static int lifePts[NUM_PLAYERS];
static LocationID *possibleMoves;
static int visited[NUM_MAP_LOCATIONS];
static int didHealthDrop[NUM_PLAYERS];
static int *DraculasPossibleMoves;
static int numDracLocations;
static int currLoc;

void decideHunterMove(HunterView hv)
{
	int round = giveMeTheRound(hv);
	int roundMod = round % (NUM_PLAYERS-1);
    int player = whoAmI(hv);

   // check the health of all players including drac & put in lifePts[] array
    int i = 0;
    for ( i = 0; i < NUM_PLAYERS; i++ ) {
    	lifePts[i] = howHealthyIs(hv, i);
    }


    if ( round == 0 ) {
     	// start the game with each hu./csenter in specific location
     	switch (player) {
			case PLAYER_LORD_GODALMING : registerBestPlay("GA", "Godalming Here"); break;
	        case PLAYER_DR_SEWARD : registerBestPlay("VI", "Seward Here"); break;
	        case PLAYER_VAN_HELSING : registerBestPlay("PA", "Helsing Here"); break;
	        case PLAYER_MINA_HARKER : registerBestPlay("MA", "HARKER Here"); break;
    	}
    	return;

    } else { // round 1 up

	    // get dracTrail
	    giveMeTheTrail(hv, PLAYER_DRACULA , dracTrail);

	    if (isValidLoc(dracTrail[0])){
			int *numDracLocationsPtr = &numDracLocations;
			DraculasPossibleMoves = whereCanTheyGo(hv, numDracLocationsPtr, PLAYER_DRACULA, TRUE, FALSE, TRUE);
				printf("\nDraculasPossibleMoves are: ");
				for ( i = 0; i < numDracLocations; i++) printf("%s ",idToAbbrev(DraculasPossibleMoves[i]) );
				printf("\n");
			
		}

		printf("DracTrail: ");
		for ( i = 0; i < TRAIL_SIZE; i++ ){

			if (isValidTrail(dracTrail[i]))	printf("[%s] <- ", idToAbbrev(dracTrail[i]));

			// get the last location drac was seen at
			if ( isValidLoc(dracTrail[i]) ) { 
				if(DEBUGGING) printf("  Drac is around  %s\n", idToAbbrev(dracTrail[i]));
				attack(hv, dracTrail[i] , player, roundMod);  
				return;
			}
		} 

	    // we can reveal Dracula's 6th move (of round 5) in dracTrail if we rest all the hunters
	    int rnd = round % 6;
	    switch (rnd) {
	    	case 0: 	// round 6 12 18 ..etc
	    		restHunter(hv, player); 
	    		for ( i = 0; i < NUM_PLAYERS; i++ ) { visited[i] = FALSE; }
	    		break;
	    		// 0th index of dracTrail is his current location, e.g in round 5 & now it will be revealed in round 6
	    		// but he gets to move before we take our next move
	    	case 1:		// dracula is found (round 7 13 19.. etc)
	    		if (isValidLoc(dracTrail[0])) {
	    			attack(hv , dracTrail[0] , player, roundMod);
	    		} else{
	    			//printf("INVALID dracTrail: %d\n", dracTrail[0]);
	    			goto weLostHim; // if 6 rounds have passed without confronting drac No valid location will be in the trail
	    		} break;
	    	weLostHim:	//continue the normal search in all other rounds - 1 2 3 4 5 - - 8 9 10 11 - - 14...
 			default : mapSearch(hv, roundMod , player); 
	    } 
	}

}

// returns the current locationID of a hunter
int getCurrLoc(HunterView hv, int player) {
	if ( didHealthDrop[player] && lifePts[player] == 9 ) {
		didHealthDrop[player] = FALSE;
		return ST_JOSEPH_AND_ST_MARYS;
	} else {
		return whereIs(hv, player);
	}
}

void mapSearch(HunterView hv , int roundMod , int player) {
	possibleMoves = (LocationID *)(malloc(sizeof(LocationID)*NUM_MAP_LOCATIONS));
	char *move;
	int n, i = 0; 
	int *numLocations = &n;

	if ( lifePts[player] <= LIFETHRESHOLD ) { restHunter(hv, player); return; }

	for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
		possibleMoves[i] = NOWHERE;
		visited[i] = FALSE;
	} 

		//fill the array with all possible legal moves
	possibleMoves = whereCanIgo(hv, numLocations, TRUE, TRUE, TRUE);

		//get current locationsID of hunters
	currLoc = whereIs(hv, player);
	printf("currLoc =  %s\n", idToAbbrev(currLoc));

		// set an initial move as the current location
	move = idToAbbrev(currLoc);

	for (i = 0; i < n; i++) {

		if ( isValidLoc(possibleMoves[i]) && possibleMoves[i] != currLoc
						 && visited[i] == FALSE ) {
			move = idToAbbrev(possibleMoves[i]);
			visited[i] = TRUE;
			break;
		}
	}

	// register the move wether found new bestMove or it is still currLoc
	switch (player) { //or simply replace this switch with registerBestPlay(move, "im here");
		case PLAYER_LORD_GODALMING : registerBestPlay(move, "Godalming Here"); break;
        case PLAYER_DR_SEWARD : registerBestPlay(move, "Seward Here"); break;
        case PLAYER_VAN_HELSING : registerBestPlay(move, "Helsing Here"); break;
        case PLAYER_MINA_HARKER : registerBestPlay(move, "HARKER Here"); break;
	}

	free (possibleMoves);
} 


void restHunter(HunterView hv, int hunterID) {
		char *restingLoc = idToAbbrev(whereIs(hv, hunterID));
		if (DEBUGGING) printf("resting at %s\n",restingLoc);
		registerBestPlay(restingLoc, "ZzzzZzzz");
}


void attack(HunterView hv, LocationID dracFoundLoc, PlayerID player , int roundMod) {
	//printf("Attacking %s\n", idToName(dracFoundLoc));
	//mapSearch(hv, roundMod , player); //remove me & the passed int roundMod from all called attack() instances

	int i, j, x = 0;
	int *nLocs = &x;
/*	LocationID *player0PossibleMoves;
	LocationID *player1PossibleMoves;
	LocationID *player2PossibleMoves;
	LocationID *player3PossibleMoves;
*/
	LocationID *huntersPossibleMoves = malloc( NUM_MAP_LOCATIONS * sizeof(LocationID));
	memset(huntersPossibleMoves, -1, NUM_MAP_LOCATIONS*sizeof(LocationID));


	// fill the current hunter's possibleMoves Array
	huntersPossibleMoves = whereCanTheyGo(hv, nLocs, player, TRUE, TRUE, TRUE);	

	// create a Map representaton of europe
	Map map = newMap(); 

	currLoc = whereIs(hv, player);

		printf("currLoc =  %s\n", idToAbbrev(currLoc));
	LocationID *pathToDrac;
	int len;
	int *pathLength = &len;
	//find the shortest path to get the current hunter to dracula

	pathToDrac = getShortestPath(map,currLoc,dracFoundLoc,pathLength);


	if ( len > 0 ) { // if path array is not empty
		if (DEBUGGING) {
			printf("\nPath is %d edges: ",len);
			for ( i = 0 ; i < len; i++ ) printf("%s-->",idToAbbrev(pathToDrac[i])); 
		}
		//move the hunter thru the path one step/turn
		LocationID moveID = pathToDrac[0];

		// see if we can do rail multi move
		i = 0; int fasterMove = FALSE;
		 // reading the path from the closest place to destination down to 1 place before currLoc
		puts("");
		for ( j = len-1; j > 0; j-- ) {
			i=0;
			while ( i < x && !fasterMove) {
				if (isValueInArray(pathToDrac[j], huntersPossibleMoves, x)) {
				moveID = pathToDrac[j];
				break;
			}
				i++;
			}
			if (fasterMove) break;
		}

		char *move =  idToAbbrev(moveID);
		if (isValidLoc(moveID)) registerBestPlay(move, "Coming for you drac");
		free(huntersPossibleMoves);
		free(pathToDrac);
		return;	
	} 
	//else
	mapSearch(hv,roundMod,player);
}

// takes locationID and returns 1 if it is valid location
int isValidLoc(int locID) {
	if ( locID >= 0 && locID <= 70 ) return 1;
	return 0;
}
/*
// send Hunters to cities aroud the location dracula was found in
LocationID whereShallIgo(HunterView hv, PlayerID player, LocationID dracFoundLoc,) {
	
}
*/

// takes locationID and returns 1 if it is valid location
int isValidLoc(int locID) {
	if ( locID >= 0 && locID <= 70 ) return 1;
	return 0;
}

// returns true if dractrail is valid i.e not -1
int isValidTrail(int trail) {
	if ( (trail >= 0 && trail <= 70) || (trail >= 100 && trail <= 108)) return 1;
	return 0;
}

//checks if a value is in an array
int isValueInArray(int val, int *arr, int size){
    int i;
    for (i=0; i < size; i++) {
        if (arr[i] == val)
            return TRUE;
    }
    return FALSE;
}

// find a path between two locations using breadth-first traversal

LocationID *getShortestPath(Map map, LocationID currLoc, LocationID dest, int *numEdges) {

	assert(map != NULL);
	int numOfNeighbers; // number of neighbors
	int *numLocs = &numOfNeighbers;
	LocationID *neighbors;
	LocationID visited[NUM_MAP_LOCATIONS];
	LocationID pred[NUM_MAP_LOCATIONS];	
	LocationID dist[NUM_MAP_LOCATIONS];

	assert(map != NULL);

 	if (DEBUGGING) printf("Finding path from %s %s..\n",idToAbbrev(currLoc) ,idToAbbrev(dest));

 	LocationID *path = malloc(NUM_MAP_LOCATIONS * sizeof(LocationID)); 

	//initialise the visited array with all locations set to not visited
	int i;
	for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
		visited[i] = FALSE;
		pred[i] = -1;
		dist[i] = 0;
	}


	visited[currLoc] = TRUE;
	Queue q = newQueue(); 
	QueueJoin(q, currLoc);

	// traverse thru the map looking for destination & skipping visited locations
	LocationID v, curr = currLoc;
	while (!QueueIsEmpty(q)) {
		curr = QueueLeave(q);
		// get the neighbor locations of current location
		neighbors = reachableLocations(map, numLocs, curr, TRUE, TRUE, TRUE, TRUE);
		visited[curr] = TRUE;
		// iterate thru the neighbors
 		for (i=0; i < numOfNeighbers; i++) {
 			v = neighbors[i];
		    if (!visited[v] && pred[v] == -1 ) {
		    	QueueJoin(q,v);
	            pred[v] = curr;
	            dist[v] = dist[curr]+1;
	        }
         }
		if (curr == dest) {
			int length = dist[curr]-1;
			//fill up the array starting from highest index
	    	path[dist[curr]] = '\0';
	        for (v = dest; v != currLoc ; v = pred[v] ) path[length--] = v;
	        dropQueue(q);
	        *numEdges =  dist[dest];	
	        return path;
 		}
 	} 
} 


