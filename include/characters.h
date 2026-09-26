#ifndef CHARACTERS_H
#define CHARACTERS_H

#include "constraints.h"

enum SCPObjectClass {
    SAFE,
    EUCLID,
    KETER
};

typedef struct {
    const unsigned id;
    const char* name;
    const char* description;
    SCPObjectClass objectClass;
} SCP;

typedef struct {
    const char* text;
    const unsigned linksTo;
} ConvoOption;

typedef struct {
    const unsigned id;
    const char* text;
    const char** options;
} ConvoNode;

typedef struct {
    const unsigned id;
    const ConvoNode* nodes;
} Conversation;

typedef struct {
    const unsigned id;
    const char* name;
    const char* description;
    const role: ; // TODO Should this be a string or an enum?
    const Conversation* conversations;
} Person;

#endif
