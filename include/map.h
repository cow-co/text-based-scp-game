#ifndef MAP_H
#define MAP_H


// TODO Merge Containment and Room (union or same struct whatever?)
typedef struct {
    unsigned id;    // TODO Remove? Instead IDs are just indices in the arrays?
    char[64] name;
    char[1024] description;
    unsigned[8] characters; 
    //interactables: ;  // TODO make a struct for these, and put the array here
    unsigned[8] connections;
} Room;

typedef struct {
    unsigned id;
    char[64] name;
    char[1024] description;
    unsigned scpID;
} Containment;

typedef struct {
    Room[32] rooms;
    unsigned[4] entranceRooms;
} Facility;

extern Facility siteFacility;

int parse_facility(const char* const filePath);
void print_facility();
void traverse_room(const unsigned roomId);

#endif
