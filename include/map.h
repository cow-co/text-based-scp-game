#ifndef MAP_H
#define MAP_H
#include <stddef.h>


typedef struct {
    char name[64];
    char description[1024];
    unsigned characters[8]; 
    unsigned interactables[8];
    unsigned containedSCP; // XXX Should probably make these ID refs into size_t (won't ever practically be a problem though)
    unsigned connections[8];
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

void clear_facility();
int parse_facility(const char* const filePath);
void print_facility();

typedef struct {
    char name[64];
    char inspectText[256];
    char interactText[256];
} Interactable;
extern Interactable allInteractables[64];

void clear_interactables();
int parse_interactables(const char* const filePath);

#endif
