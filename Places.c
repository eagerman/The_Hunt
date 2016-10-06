// Places.c ... implementation of Places

#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Places.h"

typedef struct Place {
   char      *name;
   char      *abbrev;
   LocationID id;  //int
   PlaceType  type;
} Place;

// Places should appear in alphabetic order
// Each entry should satisfy (places[i].id == i)
// First real place must be at index MIN_MAP_LOCATION
// Last real place must be at index MAX_MAP_LOCATION
static Place places[] =
{
   {"Adriatic Sea", "AS", ADRIATIC_SEA, SEA},
   {"Alicante", "AL", ALICANTE, LAND},
   {"Amsterdam", "AM", AMSTERDAM, LAND},
   {"Athens", "AT", ATHENS, LAND},
   {"Atlantic Ocean", "AO", ATLANTIC_OCEAN, SEA},
   {"Barcelona", "BA", BARCELONA, LAND},
   {"Bari", "BI", BARI, LAND},
   {"Bay of Biscay", "BB", BAY_OF_BISCAY, SEA},
   {"Belgrade", "BE", BELGRADE, LAND},
   {"Berlin", "BR", BERLIN, LAND},
   {"Black Sea", "BS", BLACK_SEA, SEA},
   {"Bordeaux", "BO", BORDEAUX, LAND},
   {"Brussels", "BU", BRUSSELS, LAND},
   {"Bucharest", "BC", BUCHAREST, LAND},
   {"Budapest", "BD", BUDAPEST, LAND},
   {"Cadiz", "CA", CADIZ, LAND},
   {"Cagliari", "CG", CAGLIARI, LAND},
   {"Castle Dracula", "CD", CASTLE_DRACULA, LAND},
   {"Clermont-Ferrand", "CF", CLERMONT_FERRAND, LAND},
   {"Cologne", "CO", COLOGNE, LAND},
   {"Constanta", "CN", CONSTANTA, LAND},
   {"Dublin", "DU", DUBLIN, LAND},
   {"Edinburgh", "ED", EDINBURGH, LAND},
   {"English Channel", "EC", ENGLISH_CHANNEL, SEA},
   {"Florence", "FL", FLORENCE, LAND},
   {"Frankfurt", "FR", FRANKFURT, LAND},
   {"Galatz", "GA", GALATZ, LAND},
   {"Galway", "GW", GALWAY, LAND},
   {"Geneva", "GE", GENEVA, LAND},
   {"Genoa", "GO", GENOA, LAND},
   {"Granada", "GR", GRANADA, LAND},
   {"Hamburg", "HA", HAMBURG, LAND},
   {"Ionian Sea", "IO", IONIAN_SEA, SEA},
   {"Irish Sea", "IR", IRISH_SEA, SEA},
   {"Klausenburg", "KL", KLAUSENBURG, LAND},
   {"Le Havre", "LE", LE_HAVRE, LAND},
   {"Leipzig", "LI", LEIPZIG, LAND},
   {"Lisbon", "LS", LISBON, LAND},
   {"Liverpool", "LV", LIVERPOOL, LAND},
   {"London", "LO", LONDON, LAND},
   {"Madrid", "MA", MADRID, LAND},
   {"Manchester", "MN", MANCHESTER, LAND},
   {"Marseilles", "MR", MARSEILLES, LAND},
   {"Mediterranean Sea", "MS", MEDITERRANEAN_SEA, SEA},
   {"Milan", "MI", MILAN, LAND},
   {"Munich", "MU", MUNICH, LAND},
   {"Nantes", "NA", NANTES, LAND},
   {"Naples", "NP", NAPLES, LAND},
   {"North Sea", "NS", NORTH_SEA, SEA},
   {"Nuremburg", "NU", NUREMBURG, LAND},
   {"Paris", "PA", PARIS, LAND},
   {"Plymouth", "PL", PLYMOUTH, LAND},
   {"Prague", "PR", PRAGUE, LAND},
   {"Rome", "RO", ROME, LAND},
   {"Salonica", "SA", SALONICA, LAND},
   {"Santander", "SN", SANTANDER, LAND},
   {"Saragossa", "SR", SARAGOSSA, LAND},
   {"Sarajevo", "SJ", SARAJEVO, LAND},
   {"Sofia", "SO", SOFIA, LAND},
   {"St Joseph and St Marys", "JM", ST_JOSEPH_AND_ST_MARYS, LAND},
   {"Strasbourg", "ST", STRASBOURG, LAND},
   {"Swansea", "SW", SWANSEA, LAND},
   {"Szeged", "SZ", SZEGED, LAND},
   {"Toulouse", "TO", TOULOUSE, LAND},
   {"Tyrrhenian Sea", "TS", TYRRHENIAN_SEA, SEA},
   {"Valona", "VA", VALONA, LAND},
   {"Varna", "VR", VARNA, LAND},
   {"Venice", "VE", VENICE, LAND},
   {"Vienna", "VI", VIENNA, LAND},
   {"Zagreb", "ZA", ZAGREB, LAND},
   {"Zurich", "ZU", ZURICH, LAND},
};

// given a Place number, return its name
char *idToName(LocationID p)
{
   assert(validPlace(p));
   return places[p].name;
}

// given a Place number, return its type
int idToType(LocationID p)
{
   assert(validPlace(p));
   return places[p].type;
}

// given a Place name, return its ID number
// binary search
int nameToID(char *name)
{
   int lo = MIN_MAP_LOCATION, hi = MAX_MAP_LOCATION;
   while (lo <= hi) {
      int mid = (hi+lo)/2;
      int ord = strcmp(name,places[mid].name);
      if (ord < 0)
         hi = mid-1;
      else if (ord > 0)
         lo = mid+1;
      else
         return places[mid].id;
   }
   return NOWHERE;
}

// given a Place abbreviation (2 char), return its ID number
int abbrevToID(char *abbrev)
{
   // an attempt to optimise a linear search
   Place *p;
   Place *first = &places[MIN_MAP_LOCATION];
   Place *last = &places[MAX_MAP_LOCATION];
   for (p = first; p <= last; p++) {
      char *c = p->abbrev;
      if (c[0] == abbrev[0] && c[1] == abbrev[1] && c[2] == '\0') return p->id;
   }
   return NOWHERE;
}

// converts ID (number) to abbreviation which are two characters
char *idToAbbrev (LocationID id) {
	char *abbrv;
	//assert (validPlace(p));
	//check struct in Places.c and Places.h for clarification
	if ((id >= 0) && (id <= 70)) {
		abbrv = places[id].abbrev; 
	} else if (id == 100) {
		abbrv = "C?"; //CITY_UNKNOWN 
	} else if (id == 101) {
		abbrv = "S?"; //SEA_UNKNOWN
	} else if (id == 102) {
		abbrv = "HI"; 
	} else if (id == 103) {
		abbrv = "D1"; 
	} else if (id == 104) {
		abbrv = "D2"; 
	} else if (id == 105) {
		abbrv = "D3"; 
	} else if (id == 106) {
		abbrv = "D4";
	} else if (id == 107) {
		abbrv = "D5";
	} else if (id == 108) {
		abbrv = "TP"; 
	} else { //something went wrong
		printf("Something fked up\n"); 
		abbrv = "??";
	}
	return abbrv; 
}

