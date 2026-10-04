#ifndef CHARACTERS_H
#define CHARACTERS_H

typedef enum {
    SAFE = 0,
    EUCLID = 1,
    KETER = 2
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
    char text[256];
    unsigned linksTo;
} ConvoOption;

typedef struct {
    unsigned id;
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

typedef struct {
    unsigned id;
    char name[64];
    char description[1024];
    Conversation conversations[16];
} Person;

extern Person allPeople[32];
int parse_people(const char* const filePath);
void print_people();

#endif
