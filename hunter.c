// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

/*
- try to keep the score as high as possible
- try to hunt down vampires before they mature. loosing 2lifepts better than 13 scorePts on each mature vampire
- hunter should rest (3 lifePts) as soon as their blood level approach 
- what happenes after hospital?
- if u encounter an immature vampire u kill him for free :)
- if u encounter a trap u destroy it and loose 2 lifePts

 the map: https://cgi.cse.unsw.edu.au/~cs1927/16s2/ass/ass2/Pics/map2.png

- tracing dracula:
	- on round 6 if all hunters rest they will know where dracula was on prev turn (6th loc in trail)
	- if one hunter meets drac in a city all other hunters can see his location in his trail
	- but NOTE: even if a Hunter is already at the location Dracula has moved to, no encounter occurs 
		at this time, as dealing with encounters only happens in the action part of a Hunter's turn
		so the hunter has to wait for his next turn to confront drac
		while if u enter a city drac is in you can encounter his traps and vamps and confront him in the current turn
	- keep an eye on drac if he goes to sea or land from sea, we can then follow him
	- so we want to read drac's trail on every round
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Game.h"
#include "HunterView.h"
#include "hunter.h"

#define LIFETHRESHOLD 5


void restAllHunters();
void restHunter(HunterView hv, int num);
void mapSearch(HunterView hv , int roundMod, int player);
void attack(HunterView hv, LocationID city);

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
    int dracFoundLoc;

   // check the health of all players including drac & put in lifePts[] array
    int i = 0;
    for ( i = 0; i < NUM_PLAYERS; i++ ) {
    	lifePts[i] = howHealthyIs(hv, i);
    }

    // get dracTrail
    giveMeTheTrail(hv, PLAYER_DRACULA , dracTrail);

	if (dracTrail[0] == CASTLE_DRACULA) { attack(hv, CASTLE_DRACULA); return; }

    if ( round == 0 ) {
     	// start the game with each hunter in specific location
     	switch (player) {
			case PLAYER_LORD_GODALMING : registerBestPlay("GA", "Godalming Here"); break;
	        case PLAYER_DR_SEWARD : registerBestPlay("VI", "Seward Here"); break;
	        case PLAYER_VAN_HELSING : registerBestPlay("PA", "Helsing Here"); break;
	        case PLAYER_MINA_HARKER : registerBestPlay("MA", "HARKER Here"); break;
    	}
    return;

    } else { // round 1 up
	    	mapSearch(hv, roundMod , player);
	    
	    // we can reveal Dracula's 6th move (of round 5) in dracTrail if we rest all the hunters
	    if ( round % 6 == 0 ) { restAllHunters(hv); // round 6 12 18 ..etc
	    	dracFoundLoc = dracTrail[0];
	    	// 0th index of dracTrail is his current location, e.g in round 5 & now it will be revealed in round 6
	    	// but he gets to move before we take our next move

	    } else if ( round % 6 == 1 ) { //dracula is found (round 7 13 19.. etc)
	    	// get all the hunters to move towards dracula. he is only one move ahead
	    	attack(hv , dracFoundLoc);

		} else { // all other rounds - 1 2 3 4 5 - - 8 9 10 11 - - 14...

			mapSearch(hv, roundMod , player);
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

	//get current locations of hunters
	int currLoc = whereIs (hv, player);

		// printing the possibleMoves
 /*	for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
		if (possibleMoves[i] != NOWHERE) printf("%s ",idToAbbrev(possibleMoves[i])); 
	}
*/	

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


void restAllHunters(HunterView hv) {
	int playerNum;
	for ( playerNum = 0; playerNum < NUM_PLAYERS-1; playerNum++ ) {
		char *restingLoc = idToAbbrev(whereIs(hv, playerNum));
		registerBestPlay( restingLoc, "where are you drac?");
	}
}

void restHunter(HunterView hv, int hunterID) {
		char *restingLoc = idToAbbrev(whereIs(hv, hunterID));
		registerBestPlay( restingLoc, "ZzzzZzzz");
}

void attack(HunterView hv, LocationID city) {
	//TODO
	/*
	send all the hunters to this location using the shortest path
	use Dijkstra's algorithm or other efficient algo
	take into consideration the formula that i misundertood before
	(round+playerNum)%4
	e.g for player 2
	(round 2 + 2 ) % 4 = 0 No rail
	(round 3 + 2 ) % 4 = 1 rail links
	(round 0 + 2 ) % 4 = 2 rail links
	(round 1 + 2 ) % 4 = 3 rail links
*/

}