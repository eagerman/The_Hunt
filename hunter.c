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
        if(player == PLAYER_LORD_GODALMING) { 
            registerBestPlay("GE", "Godalming Here"); 
        } else if (player == PLAYER_DR_SEWARD) {
            registerBestPlay("GE", "Seward Here");
        } else if(player == PLAYER_VAN_HELSING) {
            registerBestPlay("GE", "Helsing Here");
        } else if(player == PLAYER_MINA_HARKER) {
            registerBestPlay("GE", "Harker Here");
        }
    } else { // round 1 upwards
		int i = 0; 
		for (i = 0; i < NUM_MAP_LOCATIONS; i++){
			possibleMoves[i] = -1;

		} 
		possibleMoves = whereCanIgo(gameState, &numLocations, TRUE, TRUE, TRUE);
		
		//get current locations of hunters
		int currLoc = whereIs (gameState, player);
		
		
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

   		// we can reveal Dracula's 6th move after round 5, if we rest all the hunters
		if ( round == 5 ){
			//rest the hunters
			resetAllHunters();
			bestMove = traceDrac();
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