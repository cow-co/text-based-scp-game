#include "constraints.h"

typedef struct {
    id: unsigned;
    name: char[];
    description: char[];
    characters: unsigned[]; 
    interactables: ;  // TODO make a struct for these, and put the array here
    connections: unsigned[];
} Room;

typedef struct {
    id: unsigned;
    name: char[];
    description: char[];
    scpID: unsigned;
} Containment;

typedef struct {
    rooms: Room[];
    entranceRooms: unsigned[];
} Facility;
