#ifndef MAP_H
#define MAP_H
#include <stddef.h>


typedef struct {
    char name[64];
    char description[1024];
    size_t characters[8]; 
    size_t interactables[8];
    size_t connections[8];
    unsigned repairCost;
    size_t firstEntrySetsFlag;
    size_t firstEntryUnsetsFlag;
    size_t requiresFlagSet;
    size_t requiresFlagUnset;
    bool entered;
} Room;

typedef struct {
    Room rooms[32];
    size_t entranceRooms[4];
} Facility;

extern Facility siteFacility;

int parse_facility(const char* const filePath);
void print_facility();

typedef struct {
    char name[64];
    char inspectText[256];
    char interactText[256];
    size_t requiresFlag;
    size_t unrequiresFlag;
    size_t setsFlag;
    size_t unsetsFlag;
} Interactable;
extern Interactable allInteractables[64];

int parse_interactables(const char* const filePath);

#endif
