#include "constants.h"

size_t maxConversations = 32;

typedef struct {
    id: unsigned;
    name: char[nameMaxLen];
    description: char[descMaxLen];
} SCP;

typedef struct {

} ConvoOption;

typedef struct {

} ConvoNode;

typedef struct {
    
} Conversation;

typedef struct {
    id: unsigned;
    name: char[nameMaxLen];
    description: char[descMaxLen];
    role: ;
    conversations: Conversation[maxConversations];
} Person;
