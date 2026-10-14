#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <stddef.h>

typedef struct {
    char text[256];
    unsigned linksTo;
} ConvoOption;

typedef struct {
    char text[1024];
    ConvoOption options[8];
    unsigned setsFlag;
    unsigned unsetsFlag;
    unsigned requiresFlagSet;
    unsigned requiresFlagUnset;
} ConvoNode;

typedef struct {
    ConvoNode nodes[32];
} Conversation;

extern Conversation allConvos[512]; // enough to hold max convos for max number of persons
int parse_convos(const char* const filePath);
void print_all_convos();

#endif
