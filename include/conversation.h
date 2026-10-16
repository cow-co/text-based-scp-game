#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <stddef.h>

typedef struct {
    char text[256];
    size_t linksTo;
} ConvoOption;

typedef struct {
    char text[1024];
    ConvoOption options[8];
    size_t setsFlag;
    size_t unsetsFlag;
    size_t requiresFlagSet;
    size_t requiresFlagUnset;
} ConvoNode;

typedef struct {
    ConvoNode nodes[32];
} Conversation;

extern Conversation allConvos[512]; // enough to hold max convos for max number of persons
int parse_convos(const char* const filePath);
void print_all_convos();

#endif
