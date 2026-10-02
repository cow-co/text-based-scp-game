#ifndef CHARACTERS_H
#define CHARACTERS_H

typedef enum {
    SAFE,
    EUCLID,
    KETER
} SCPObjectClass;

typedef struct {
    unsigned id;
    char name[64];
    char description[8192];
    SCPObjectClass objectClass;
} SCP;

extern SCP allSCPs[32]; // Not literally ALL SCPs obviously, but all the ones available in the game
int parse_scps(const char* const filePath);
void print_scps();

typedef struct {
    const char text[256];
    const unsigned linksTo;
} ConvoOption;

typedef struct {
    const unsigned id;
    const char text[1024];
    const ConvoOption options[8];
} ConvoNode;

typedef struct {
    const unsigned id;
    const ConvoNode nodes[32];
} Conversation;

typedef struct {
    const unsigned id;
    const char name[64];
    const char description[1024];
    const Conversation conversations[16];
} Person;

#endif
