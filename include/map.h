#ifndef MAP_H
#define MAP_H
#include <stddef.h>


typedef struct {
    char name[64];
    char description[1024];
    unsigned characters[8]; 
    //interactables: ;  // TODO make a struct for these, and put the array here
    unsigned containedSCP; // XXX Should probably make these ID refs into size_t
    unsigned connections[8];
    unsigned firstEntrySetsFlag;
    unsigned firstEntryUnsetsFlag;
} Room;

typedef struct {
    Room rooms[32];
    unsigned entranceRooms[4];
} Facility;

extern Facility siteFacility;

int parse_facility(const char* const filePath);
void print_facility();

#endif
