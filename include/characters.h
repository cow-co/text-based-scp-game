#include "constraints.h"

typedef struct {
    id: unsigned;
    name: char[];
    description: char[];
} SCP;

typedef struct {
    text: char[];
    linksTo: unsigned;
} ConvoOption;

typedef struct {
    id: unsigned;
    text: char[];
    options: char[][];
} ConvoNode;

typedef struct {
    id: unsigned;
    nodes: ConvoNode[];
} Conversation;

typedef struct {
    id: unsigned;
    name: char[];
    description: char[];
    role: ; // TODO Should this be a string or an enum?
    conversations: Conversation[];
} Person;
