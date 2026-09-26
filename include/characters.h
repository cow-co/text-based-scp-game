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
    const char[64] name;
    const char[8192] description;
    SCPObjectClass objectClass;
} SCP;

typedef struct {
    const char[256] text;
    const unsigned linksTo;
} ConvoOption;

typedef struct {
    const unsigned id;
    const char[1024] text;
    const ConvoOption[8] options;
} ConvoNode;

typedef struct {
    const unsigned id;
    const ConvoNode[32] nodes;
} Conversation;

typedef struct {
    const unsigned id;
    const char[64] name;
    const char[1024] description;
    const role: ; // TODO Should this be a string or an enum?
    const Conversation[16] conversations;
} Person;

#endif
