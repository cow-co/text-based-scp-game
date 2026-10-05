#ifndef CHARACTERS_H
#define CHARACTERS_H

#include "conversation.h"

typedef enum {
    SAFE = 0,
    EUCLID = 1,
    KETER = 2
} SCPObjectClass;

typedef struct {
    unsigned id;
    char name[64];
    char description[8192];
    SCPObjectClass objectClass;
} SCP;

extern SCP allSCPs[32]; // Not literally ALL SCPs obviously, but all the ones available in the game
int parse_scps(const char* const filePath);
void print_scps();

typedef struct {
    unsigned id;
    char name[64];
    char description[1024];
    char role[64];
    unsigned conversations[16];
} Person;

extern Person allPeople[32];
int parse_people(const char* const filePath);
void print_people();

#endif
