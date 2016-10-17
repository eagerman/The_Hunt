// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "DracView.h"
#include "Places.h"

#define SIZE 16

void decideDraculaMove(DracView gameState) { 
   // char *possibleMove[SIZE] = {"SJ","ZA","SZ","KL","CD","GA","BC","SO","SA","D5"};
    char *possibleMove[SIZE] = {"BR","PR","VI","BD","KL","CD","GA","BC","SO","SA","VA","SJ","ZA","MU","NU","LI"};
    registerBestPlay(possibleMove[giveMeTheRound(gameState)%SIZE], "sucker");    
}
	 	  		