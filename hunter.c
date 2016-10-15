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
#include <regex.h>
#include <string.h>
#include "Game.h"
#include "HunterView.h"
#include "hunter.h"

#define LIFETHRESHOLD 5
#define DEBUGGING 0

void restAllHunters();
void restHunter(HunterView hv, int num);
void mapSearch(HunterView hv , int roundMod, int player);
void attack(HunterView hv, LocationID dracFoundLoc, int player , int roundMod);
int isMatching(const char *string, char *pattern);
int isValidLoc( char *dracTrailAbbrev );

//Global Variables
static LocationID dracTrail[TRAIL_SIZE];
static int lifePts[NUM_PLAYERS];
static int numLocations = NUM_MAP_LOCATIONS;
static LocationID *possibleMoves;


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

	    // when a hunter confronts drac it will show up in his trail as 2 strings like "AZ"
		// but we need to differenciate that from "C?" "S?" "HI" && "Dn"
		char *dracTrailAbbrev = idToAbbrev(dracTrail[0]);

		if (isValidLoc(dracTrailAbbrev)) {
			printf("Matched %s or %d\n",dracTrailAbbrev, dracTrail[0] );
			attack(hv, dracTrail[0] , player, roundMod);  
			return; // uncomment me when you implement attack() 
		}

	    if (DEBUGGING) printf("Round %d & dracTrailAbbrev = %s | " , round , dracTrailAbbrev);

	    // we can reveal Dracula's 6th move (of round 5) in dracTrail if we rest all the hunters
	    int rnd = round % 6;
	    switch (rnd) {
	    	case 0: 	// round 6 12 18 ..etc
	    		restHunter(hv, player); 
	    		break;
	    		// 0th index of dracTrail is his current location, e.g in round 5 & now it will be revealed in round 6
	    		// but he gets to move before we take our next move
	    	case 1:		// dracula is found (round 7 13 19.. etc)
	    		if (isValidLoc(dracTrailAbbrev)) {
	    			attack(hv , dracTrail[0] , player, roundMod);
	    		} else{
	    			//printf("INVALID dracTrail: %d\n", dracTrail[0]);
	    			goto inValid;
	    		} break;
	    	inValid:	
 			default : mapSearch(hv, roundMod , player); // all other rounds - 1 2 3 4 5 - - 8 9 10 11 - - 14...
	    } 
	}
}


void mapSearch(HunterView hv , int roundMod , int player) {
	possibleMoves = (LocationID *)(malloc(sizeof(LocationID)*NUM_MAP_LOCATIONS));
	int bestMove = 0;
	char *move;
	int i = 0; 

	if ( lifePts[player] <= LIFETHRESHOLD ) { restHunter(hv, player); return; }

	for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
		possibleMoves[i] = NOWHERE;
	} 
	//fill the array with all possible legal moves
	possibleMoves = whereCanIgo(hv, &numLocations, TRUE, TRUE, TRUE);

	//get current locationsID of hunters
	int currLoc = whereIs(hv, player);
	move = idToAbbrev(currLoc);

		// printing the possibleMoves
 	if (DEBUGGING) {
 		printf("Possible Moves = ");
 		for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
			if (possibleMoves[i] != NOWHERE) printf("%s ",idToAbbrev(possibleMoves[i])); 
		}
	}
	// TODO
	// replace the following with an algorithm to search the rest of the map with the other 3 hunters
	// take into account when drac is found and attacked in round%6=1 how to expect his next move inorder to keep track of him
	// always read his trail to check for HIDES or DOUBLEBACKS
	for (i = 0; i < NUM_MAP_LOCATIONS; i++) {

		if (possibleMoves[i] != NOWHERE && possibleMoves[i] != currLoc) {

			bestMove = possibleMoves[i];
			move = idToAbbrev(bestMove);
			break;
	    }
	}

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

void attack(HunterView hv, LocationID dracFoundLoc, int player , int roundMod) {
	//TODO
	/*
	send all the hunters to this location using the shortest path, and search around it
	use Dijkstra's algorithm or other efficient algo
	if it is goinf to take a hunter 3 moves to get to the city expect Drac to be 3 moves away 
	so send the hunter to the city surrounding this city and close doen on him
	take into consideration the formula that i misundertood before
	(round+playerNum)%4
	e.g for player 2
	(round 2 + 2 ) % 4 = 0 No rail
	(round 3 + 2 ) % 4 = 1 rail links
	(round 0 + 2 ) % 4 = 2 rail links
	(round 1 + 2 ) % 4 = 3 rail links
*/
	// also check that the dracFoundLoc being passed is valid place use isValidLoc()
	//temporarly
	if ( dracFoundLoc >= 1 && dracFoundLoc <= 70 ) {
		printf("ATTACKING ////////////////////////////    %s    /////////////////////////////\n", idToName(dracFoundLoc));
	} else {
		printf("ATTACKING ////////////////////////////    %d    /////////////////////////////\n", dracFoundLoc);		
	}
	mapSearch(hv, roundMod , player); //remove me & the passed int roundMod
}

// matches a string to a regular expression
int isMatching( const char *string, char *pattern ) {
    int    status;
    regex_t    re;

    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0)
        return FALSE;
    status = regexec(&re, string, (size_t) 0, NULL, 0);
    regfree(&re);
    if (status != 0) 
        return FALSE;
    return TRUE;
}

// takes location abbreviation from dracTrail and returns 1 if it is valid location
int isValidLoc( char *dracTrailAbbrev ) {
	if ( isMatching(dracTrailAbbrev,"[A-Z][A-Z]") 
			&& strcmp(dracTrailAbbrev,"HI") != 0
			&& dracTrail[0] >= 1 && dracTrail[0] <= 70 ) 
		return 1;
	return 0;
}