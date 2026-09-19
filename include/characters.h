#include "constants.h"

size_t const maxConversations = 32;
size_t const convoNodeMaxOptions = 8;
size_t const convoMaxNodes = 64;

typedef struct {
    id: unsigned;
    name: char[nameMaxLen];
    description: char[descMaxLen];
} SCP;

typedef struct {
    text: char[descMaxLen];
    linksTo: unsigned;
} ConvoOption;

typedef struct {
    id: unsigned;
    text: char[descMaxLen];
    options: char[descMaxLen][convoNodeMaxOptions];
} ConvoNode;

typedef struct {
    id: unsigned;
    nodes: ConvoNode[convoMaxNodes];
} Conversation;

typedef struct {
    id: unsigned;
    name: char[nameMaxLen];
    description: char[descMaxLen];
    role: ; // TODO Should this be a string or an enum?
    conversations: Conversation[maxConversations];
} Person;
