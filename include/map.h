#ifndef MAP_H
#define MAP_H

#include "constraints.h"

// TODO Swap to using statically-sized arrays
typedef struct {
    const unsigned id;
    const char* name;
    const char* description;
    unsigned* characters; 
    interactables: ;  // TODO make a struct for these, and put the array here
    unsigned* connections;
} Room;

typedef struct {
    const unsigned id;
    const char* name;
    const char* description;
    unsigned scpID;
} Containment;

typedef struct {
    Room* rooms;
    unsigned* entranceRooms;
} Facility;

#endif
