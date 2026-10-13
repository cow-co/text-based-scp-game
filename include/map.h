#ifndef MAP_H
#define MAP_H
#include <stddef.h>


typedef struct {
    char name[64];
    char description[1024];
    unsigned characters[8]; 
    unsigned interactables[8];
    unsigned containedSCP;
    unsigned connections[8];
    bool containmentBreached;
    unsigned repairCost;
    unsigned firstEntrySetsFlag;
    unsigned firstEntryUnsetsFlag;
    unsigned requiresFlagSet;
    unsigned requiresFlagUnset;
} Room;

typedef struct {
    Room rooms[32];
    unsigned entranceRooms[4];
} Facility;

extern Facility siteFacility;

int parse_facility(const char* const filePath);
void print_facility();
bool are_any_rooms_breached();

typedef struct {
    char name[64];
    char inspectText[256];
    char interactText[256];
    unsigned requiresFlag;
    unsigned unrequiresFlag;
    unsigned setsFlag;
    unsigned unsetsFlag;
} Interactable;
extern Interactable allInteractables[64];

int parse_interactables(const char* const filePath);

#endif
