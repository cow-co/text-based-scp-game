#ifndef CHARACTERS_H
#define CHARACTERS_H

#include "conversation.h"

typedef enum {
    SAFE = 0,
    EUCLID = 1,
    KETER = 2
} SCPObjectClass;

typedef struct {
    char name[64];
    char description[8192];
    char breachedDesc[1024];
    SCPObjectClass objectClass;
    bool contained;
    bool breached;  // ie. it's nominally in containment but has escaped
    bool containmentRepaired;
    double breachProb;
    double captureProb;
    unsigned budgetValue;
} SCP;

extern SCP allSCPs[32]; // Not literally ALL SCPs obviously, but all the ones available in the game
int parse_scps(const char* const filePath);
void print_scp_details(size_t scpID);

typedef struct {
    char name[64];
    char description[1024];
    char role[64];
    size_t conversation;
} Person;

extern Person allPeople[32];
int parse_people(const char* const filePath);
void print_people();

#endif
