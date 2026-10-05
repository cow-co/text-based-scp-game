#ifndef CONVERSATION_H
#define CONVERSATION_H

typedef struct {
    char text[256];
    unsigned linksTo;
} ConvoOption;

typedef struct {
    char text[1024];
    ConvoOption options[8];
} ConvoNode;

typedef struct {
    unsigned id;
    ConvoNode nodes[32];
} Conversation;

extern Conversation allConvos[512]; // enough to hold max convos for max number of persons
int parse_convos(const char* const filePath);
void conversation(const unsigned id);
void print_all_convos();

#endif
