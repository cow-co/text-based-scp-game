#ifndef MAP_H
#define MAP_H

#include "constraints.h"

typedef struct {
    unsigned id;
    char* name;
    char* description;
    unsigned* characters; 
    interactables: ;  // TODO make a struct for these, and put the array here
    unsigned* connections;
} Room;

typedef struct {
    unsigned id;
    char* name;
    char* description;
    unsigned scpID;
} Containment;

typedef struct {
    Room* rooms;
    unsigned* entranceRooms;
} Facility;

#endif
