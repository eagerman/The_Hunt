// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

/*
- what happenes after hospital?
 the map: https://cgi.cse.unsw.edu.au/~cs1927/16s2/ass/ass2/Pics/map2.png

- tracing dracula:
	- on round 6 if all hunters rest they will know where dracula was on prev turn (6th loc in trail)
	- if one hunter meets drac in a dracFoundLoc all other hunters can see his location in his trail
	- but NOTE: even if a Hunter is already at the location Dracula has moved to, no encounter occurs 
		at this time, as dealing with encounters only happens in the action part of a Hunter's turn
		so the hunter has to wait for his next turn to confront drac
		while if u enter a dracFoundLoc drac is in you can encounter his traps and vamps and confront him in the current turn
	- keep an eye on drac if he goes to sea or land from sea, we can then follow him
	- so we want to read drac's trail on every round
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Game.h"
#include "HunterView.h"
#include "hunter.h"
#include "Map.h"
//#include "Queue.h"

#define LIFETHRESHOLD 3
#define DEBUGGING 0

void restHunter(HunterView hv, int num);
int getCurrLoc(HunterView hv, int player);
void mapSearch(HunterView hv , int roundMod, int player);
void attack(HunterView hv, LocationID dracFoundLoc, int player , int roundMod);
int isValidLoc(int trail);
LocationID *getShortestPath(Map map, LocationID src, LocationID dest, int *length);

//Global Variables
static LocationID dracTrail[TRAIL_SIZE];
static int lifePts[NUM_PLAYERS];
static int numLocations = NUM_MAP_LOCATIONS;
static LocationID *possibleMoves;
static int visited[NUM_MAP_LOCATIONS];
static int didHealthDrop[NUM_PLAYERS];
static int *DraculasPossibleMoves;
static int numDracLocations;

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

    	// check if the hunter is in hospital
		if ( lifePts[player] < 9 ) didHealthDrop[player] = TRUE;
	    // get dracTrail
	    giveMeTheTrail(hv, PLAYER_DRACULA , dracTrail);

	    if (isValidLoc(dracTrail[0])){
			int *numDracLocationsPtr = &numDracLocations;
			DraculasPossibleMoves = whereCanTheyGo(hv, numDracLocationsPtr, PLAYER_DRACULA, TRUE, FALSE, TRUE);
			printf("\nDraculasPossibleMoves are: ");
			for ( i = 0; i < numDracLocations; i++) printf("%s ",idToAbbrev(DraculasPossibleMoves[i]) );
			printf("\n");
		}

		for ( i = 0; i < TRAIL_SIZE; i++ ){
			//if ( dracTrail[i] == UNKNOWN_LOCATION ) continue;
			if (DEBUGGING) printf("[%d]-", dracTrail[i]);

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

// returns the current location of a hunter
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
	int i = 0; 

	if ( lifePts[player] <= LIFETHRESHOLD ) { restHunter(hv, player); return; }

	for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
		possibleMoves[i] = NOWHERE;
		visited[i] = FALSE;
	} 

		//fill the array with all possible legal moves
	possibleMoves = whereCanIgo(hv, &numLocations, TRUE, TRUE, TRUE);

		//get current locationsID of hunters
	int currLoc = getCurrLoc(hv, player);

		// set an initial move as the current location
	move = idToAbbrev(currLoc);
	printf("Player%d currLoc is %s\n",player, move);

 	if (DEBUGGING) { 	// printing the possibleMoves
 		printf("Possible Moves = ");
 		for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
 			if ( possibleMoves[i] == UNKNOWN_LOCATION || !isValidLoc(possibleMoves[i]) ) continue;
			printf("%s ",idToAbbrev(possibleMoves[i])); 
		}
		 		printf("\n");
	}

	for (i = 0; i < NUM_MAP_LOCATIONS; i++) {

		if ( possibleMoves[i] != UNKNOWN_LOCATION && possibleMoves[i] != currLoc
						 && visited[i] == FALSE ) {
			move = idToAbbrev(possibleMoves[i]);
			if (DEBUGGING) printf("Setting %s as Visited\n", move);
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
		printf("resting at %s\n",restingLoc);
		registerBestPlay( restingLoc, "ZzzzZzzz");
}


void attack(HunterView hv, LocationID dracFoundLoc, PlayerID player , int roundMod) {
	//TODO
	printf("Attacking %s\n", idToName(dracFoundLoc));
	mapSearch(hv, roundMod , player); //remove me & the passed int roundMod from all called attack() instances

	int i, j, x = 0;
	int *nLocs = &x;
	LocationID *player0PossibleMoves = (LocationID *)(malloc(NUM_MAP_LOCATIONS*sizeof(LocationID)));
	LocationID *player1PossibleMoves = (LocationID *)(malloc(NUM_MAP_LOCATIONS*sizeof(LocationID)));
	LocationID *player2PossibleMoves = (LocationID *)(malloc(NUM_MAP_LOCATIONS*sizeof(LocationID)));
	LocationID *player3PossibleMoves = (LocationID *)(malloc(NUM_MAP_LOCATIONS*sizeof(LocationID)));
	LocationID *huntersPossibleMoves = (LocationID *)(malloc(NUM_MAP_LOCATIONS*sizeof(LocationID)));


	for ( i = 0; i < NUM_PLAYERS-1; i++ ) { 
		printf("Player%d possibleMoves are: ",i); 
		huntersPossibleMoves = whereCanTheyGo(hv, nLocs, i, TRUE, TRUE, TRUE);	
		switch (player) {
			case PLAYER_LORD_GODALMING: 
				for (j = 0; j < x; j++) player0PossibleMoves[j] = huntersPossibleMoves[j]; break;
			case PLAYER_DR_SEWARD: 
				for (j = 0; j < x; j++) player1PossibleMoves[j] = huntersPossibleMoves[j]; break;
			case PLAYER_VAN_HELSING: 
				for (j = 0; j < x; j++) player2PossibleMoves[j] = huntersPossibleMoves[j]; break;
			case PLAYER_MINA_HARKER: 
				for (j = 0; j < x; j++) player3PossibleMoves[j] = huntersPossibleMoves[j]; break; 
		}

		for ( j = 0; j < x; j++ ) printf("%s ", idToAbbrev(huntersPossibleMoves[j]));
		printf("\n");
	} // i did this in order to decide which player to send to which city
	// but lets first implemetn just sending all hunters to dracFoundLoc
	// we can then adjust it

	// create a Map representaton of europe
	Map map = newMap(); 

	int src = getCurrLoc(hv,player);
	LocationID *pathToDrac;
	int len;
	int *pathLength = &len;
	//find the shortest path to get the current hunter to dracula
	pathToDrac = getShortestPath(map,src,dracFoundLoc,pathLength); 

	//move the hunter thru the path one step/turn
	static int step = 0;
	step++;
	LocationID moveID = pathToDrac[step];
	char *move =  idToAbbrev(moveID);
	printf("\nHeading towards %s\n", move);

	if (isValidLoc(moveID)){
		registerBestPlay(move, "Coming for you drac");
	} else {
		mapSearch(hv,roundMod,player);
	}
 
}

// takes locationID dracTrail and returns 1 if it is valid location
int isValidLoc(int trail) {
	if ( trail >= 0 && trail <= 70 ) return 1;
	return 0;
}


// find a path between two locations using breadth-first traversal
LocationID *getShortestPath(Map map, LocationID src, LocationID dest, int *length) {
	assert(map != NULL);
	int n; // number of neighbors
	int *numLocs = &n;
	LocationID *neighbors;

	// get neighbor locations to src location of the hunter
 	neighbors = reachableLocations(map, numLocs, src, PLAYER_DRACULA, TRUE, TRUE, TRUE);

 	printf("Neighbor Locations: ");
 	int i;
 	for (i=0; i < n; i++) printf("%s ",idToAbbrev(neighbors[i]));
 		
 	//pathToDrac = malloc(n * sizeof(LocationID)); // replace n with corrent length

 	//TODO
 	//BFS
 	length = &n; // replace me
 	return neighbors; // replace me
} 

