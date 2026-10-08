#include "gameplay.h"
#include "organisation.h"
#include "map.h"
#include "characters.h"
#include "conversation.h"
#include "util.h"
#include <stdio.h>


char playerName[64] = {};
Progress progress = {};

void set_flag(size_t flagBit, const bool set) {
    if (flagBit > 64) {
        flagBit = 64;
    } else if (flagBit < 0) {
        log_error("Flag index is negative!\n");
    } else {
        progress.narrativeFlags = progress.narrativeFlags | (set << flagBit);
    }
}

bool test_flag(size_t flagBit) {
    if (flagBit > 64) {
        flagBit = 64;
    }
    if (flagBit < 1) {
        log_error("Flag index must be >= 1!\n");
        return false;
    } else {
        flagBit = flagBit - 1;
        bool set = progress.narrativeFlags & (1 << flagBit);
        return set;
    }
}

bool can_enter_room(size_t roomID) {
    if (siteFacility.rooms[roomID].requiresFlagSet || siteFacility.rooms[roomID].requiresFlagUnset) {
        const unsigned flagRequired = siteFacility.rooms[roomID].requiresFlagSet;
        const unsigned flagUnrequired = siteFacility.rooms[roomID].requiresFlagUnset;
        return (test_flag(flagRequired) && !test_flag(flagUnrequired));
    } else {
        return true;
    }
}

bool can_access_node(Conversation* convo, size_t nodeID) {
    if (convo->nodes[nodeID-1].requiresFlagSet || convo->nodes[nodeID-1].requiresFlagUnset) {
        const unsigned flagRequired = convo->nodes[nodeID-1].requiresFlagSet;
        const unsigned flagUnrequired = convo->nodes[nodeID-1].requiresFlagUnset;
        return (test_flag(flagRequired) && !test_flag(flagUnrequired));
    } else {
        return true;
    }
}

void conversation(const size_t id) {
    Conversation* convo = nullptr;
    size_t countConvos = sizeof(allConvos) / sizeof(allConvos[0]);
    if (id <= countConvos && allConvos[id-1].nodes[0].text[0] != '\0') {
        convo = &allConvos[id-1];
    } else {
        log_error("Conversation not found!\n");
        return;
    }

    bool exit = false;
    ConvoNode* node = &(convo->nodes[0]);
    size_t maxOptions = sizeof(node->options) / sizeof(node->options[0]);
    while (!exit) {
        // TODO Should probably functionise this
        if (node->setsFlag != 0) {
            printf("Setting flag %u\n", node->setsFlag);
            set_flag(node->setsFlag - 1, true);
            printf("Flags is now %lu\n", (unsigned long)progress.narrativeFlags);
        }
        if (node->unsetsFlag != 0) {
            printf("Unsetting flag %u\n", node->unsetsFlag);
            set_flag(node->unsetsFlag - 1, false);
            printf("Flags is now %lu\n", (unsigned long)progress.narrativeFlags);
        }
        printf("%s\n", node->text);
        size_t numOptions = 0;
        size_t accessibleNodes[maxOptions] = {};
        for (size_t i = 0; i < maxOptions; ++i) {
            // We don't have IDs on the options, cos we don't need them; 
            // so we just check if the option text is empty.
            if (node->options[i].text[0] == '\0') {
                break;
            }
            if (can_access_node(convo, node->options[i].linksTo)) {
                accessibleNodes[numOptions] = i;
                ++numOptions;
            }
            printf("[%zu] %s\n", i, node->options[i].text);
        }
        if (numOptions == 0) {
            break;
        }
        for (size_t i = 0; i < maxOptions; ++i) {
            printf("[%zu] %s\n", i, node->options[accessibleNodes[i]].text);
        }
        printf("[%zu] Exit\n", numOptions);

        int selection = -1;
        const int count = fscanf(stdin, "%d", &selection);
        if (count <= 0) {
            printf("Could not read input!\n");
        }
        fscanf(stdin, "%*[^\n]");
        if (selection < 0 || selection > numOptions) {
            printf("Invalid Input!\n");
        } else if (selection == numOptions) {
            exit = true;
        } else {
            const unsigned link = node->options[accessibleNodes[selection]].linksTo;
            node = &(convo->nodes[link - 1]);
        }
    }
}


void traverse_rooms(const size_t fromRoomID) {
    Room* room = nullptr;
    size_t countRooms = sizeof(siteFacility.rooms) / sizeof(siteFacility.rooms[0]);
    if (fromRoomID <= countRooms && siteFacility.rooms[fromRoomID-1].name[0] != '\0') {
        room = &siteFacility.rooms[fromRoomID-1];
    } else {
        log_error("Room not found!\n");
        return;
    }

    bool exit = false;
    size_t maxConns = sizeof(room->connections) / sizeof(room->connections[0]);
    while (!exit) {
        if (room->firstEntrySetsFlag != 0) {
            printf("Setting flag %u\n", room->firstEntrySetsFlag);
            set_flag(room->firstEntrySetsFlag - 1, true);
            printf("Flags is now %lu\n", (unsigned long)progress.narrativeFlags);
        }
        if (room->firstEntryUnsetsFlag != 0) {
            printf("Unsetting flag %u\n", room->firstEntryUnsetsFlag);
            set_flag(room->firstEntryUnsetsFlag - 1, false); 
            printf("Flags is now %lu\n", (unsigned long)progress.narrativeFlags);
        }
        printf("You are in %s\n\t%s\n", room->name, room->description);
        
        size_t numOptions = 0;
        size_t accessibleConns[maxConns] = {};
        for (size_t i = 0; i < maxConns; ++i) {
            const unsigned connectsTo = room->connections[i];
            if (connectsTo == 0) {
                break;
            }
            if (can_enter_room(connectsTo - 1)) {
                accessibleConns[numOptions] = connectsTo;
                ++numOptions;
            }
        }
        for (size_t i = 0; i < maxConns; ++i) {
            if (accessibleConns[i] == 0) {
                break;
            }
            printf("[%zu] Go to %s\n", i, siteFacility.rooms[accessibleConns[i]-1].name);
        }
        
        // TODO Add options for interacting with characters
        
        if (numOptions == 0) {
            printf("No options available...\n");
            break;
        }
        printf("[%zu] Exit Game\n", numOptions);

        int selection = -1;
        const int count = fscanf(stdin, "%d", &selection);
        if (count <= 0) {
            log_error("Could not read input!\n");
            return;
        }
        fscanf(stdin, "%*[^\n]");
        if (selection < 0 || selection > numOptions) {
            printf("Invalid Input!\n");
        } else if (selection == numOptions) {
            exit = true;
        } else {
            const unsigned selectedRoomID = accessibleConns[selection];
            printf("Going to room %d\n", selectedRoomID);
            room = &(siteFacility.rooms[selectedRoomID-1]);
        }
    }
}


void play_game() {
    traverse_rooms(1);
}
