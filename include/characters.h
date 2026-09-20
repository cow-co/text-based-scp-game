#include "constraints.h"

typedef struct {
    unsigned id;
    char name[];
    char description[];
} SCP;

typedef struct {
    char text[];
    unsigned linksTo;
} ConvoOption;

typedef struct {
    unsigned id;
    char text[];
    char options[][];
} ConvoNode;

typedef struct {
    unsigned id;
    ConvoNode nodes[];
} Conversation;

typedef struct {
    unsigned id;
    char name[];
    char description[];
    role: ; // TODO Should this be a string or an enum?
    Conversation conversations[];
} Person;
