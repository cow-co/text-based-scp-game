#include "constants.h"

size_t const facilityMaxRooms = 64;
size_t const facilityMaxEntrances = 8;
size_t const roomMaxConnections = 16;

typedef struct {
    id: unsigned;
    name: char[nameMaxLen];
    description: char[descMaxLen];
    characters: unsigned[]; // TODO size this array
    interactables: ;  // TODO make a struct for these, and put the array here
    connections: unsigned[roomMaxConnections];
} Room;

typedef struct {
    id: unsigned;
    name: char[nameMaxLen];
    description: char[descMaxLen];
    scpID: unsigned;
} Containment;

typedef struct {
    rooms: Room[facilityMaxRooms];
    entranceRooms: unsigned[facilityMaxEntrances];
} Facility;
