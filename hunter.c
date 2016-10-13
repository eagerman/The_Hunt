// hunter.c
// Implementation of your "Fury of Dracula" hunter AI


    /*
	// Psuedo Code
   foreach (move in possibleMoves(gameState)) {
      worth = evaluateMove(move, gameState);
      if (worth > threshold)
         registerBestPlay(move,Message);
      if (worth > bestWorth)
         { bestWorth = worth;  bestMove = move; }
   }
   registerBestPlay(bestMove,Message);

   best for hunters:
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
	- a good example on how drac was defeated:
		https://cgi.cse.unsw.edu.au/~cs1927ass/16s2.dracula/round04/logs/h-g085-d-g028.log.gz
   

	Drac will most likely try to get to CD by round 6 inorder to get 10lifePts
		or if he doesnt have a legal move he will get teleported to CD
	i 1st thought we keep 2 hunters at GA & KL in which they will 100% confront him on the way in or out
	and the other 2 hunters can scan the map
	but i then thought that they should not be just standing because we want to minimise vampires from maturing
	so i thought i will get these 2 hunters each to circulate 3 cities lead 2 CD, 

	*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Game.h"
#include "HunterView.h"

int traceDrac();
void resetAllHunters();

void decideHunterMove(HunterView gameState)
{
	// TODO ...
    // Replace the line below by something better
    // srand (time(NULL));
	int round = giveMeTheRound(gameState);
    int player = whoAmI(gameState);
	int numLocations = NUM_MAP_LOCATIONS;
    LocationID *possibleMoves;
	possibleMoves = (LocationID *)(malloc(sizeof(LocationID)*NUM_MAP_LOCATIONS));
	int bestMove = 0;
	char *move;

    if(round == 0){
    	// start the game with every hunter in a specific location
        if(player == PLAYER_LORD_GODALMING) { 
            registerBestPlay("GA", "Godalming Here"); // ambushing drac at his door step BC<-->GA<-->CN
        } else if (player == PLAYER_DR_SEWARD) {	// meeting drac in 1 of these 6 cities means he is goin in or out of CD
            registerBestPlay("KL", "Seward Here"); // ambushing drac at his door step BD<-->KL<-->SZ
        } else if(player == PLAYER_VAN_HELSING) {
            registerBestPlay("TO", "Helsing Here"); // will scan the south west entry MR<-->TO<-->BO
        } else if(player == PLAYER_MINA_HARKER) {
            registerBestPlay("MU", "Harker Here"); // will scan middle entry VE<-->MU<-->VI
        }
    } else if ( round == 6 ) { // we can reveal Dracula's 6th move in trail
    		// if we rest all the hunters
			resetAllHunters();
			bestMove = traceDrac();
			// spread the message to other hunters

    } else if ( round == 7 ) {
    	// get all the hunters to surround dracula
    	// BFS to get the shortest path for each hunter to go towards drac
    	surroundDrac();

	} else { // all other rounds
		int i = 0; 
		for (i = 0; i < NUM_MAP_LOCATIONS; i++){
			possibleMoves[i] = -1;

		} 
		//fill the array with all possible legal moves
		possibleMoves = whereCanIgo(gameState, &numLocations, TRUE, TRUE, TRUE);
		
		//get current locations of hunters
		int currLoc = whereIs (gameState, player);
		
		// replace the following with a better algorithm
		for (i = 0; i < NUM_MAP_LOCATIONS; i++) {

			if (possibleMoves[i] != currLoc) {

				bestMove = possibleMoves[i];
				move = idToAbbrev(bestMove);

				if(player == PLAYER_LORD_GODALMING) { 
		            registerBestPlay(move, "Godalming Here"); 
		        } else if (player == PLAYER_DR_SEWARD) {
		            registerBestPlay(move, "Seward Here");
		        } else if(player == PLAYER_VAN_HELSING) {
		            registerBestPlay(move, "Helsing Here");
		        } else if(player == PLAYER_MINA_HARKER) {
		            registerBestPlay(move, "Harker Here");
		        }
				return;
			}
			i++;
		}

		free (possibleMoves);
    }
}

int traceDrac(){
	//TODO
	// replace this code
	// find 
	return 0;
}

void resetAllHunters(){
	//TODO
}