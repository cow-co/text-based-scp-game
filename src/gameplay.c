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
    } else if (flagBit < 1) {
        log_error("Flag index must be >= 1!\n");
    } else if (set) {
        flagBit -= 1;
        progress.narrativeFlags |= 1 << flagBit;
    } else {
        flagBit -= 1;
        progress.narrativeFlags &= ~(1 << flagBit);
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
        flagBit -= 1;
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

bool can_access_node(const ConvoNode* const node) {
    if (node->requiresFlagSet || node->requiresFlagUnset) {
        return (test_flag(node->requiresFlagSet) && !test_flag(node->requiresFlagUnset));
    } else {
        return true;
    }
}

const ConvoNode* handle_node(const Conversation* const convo, const ConvoNode* node, size_t maxOptions) {
    if (node->setsFlag != 0) {
        printf("Setting flag %u\n", node->setsFlag);
        set_flag(node->setsFlag, true);
        printf("Flags is now %lu\n", (unsigned long)progress.narrativeFlags);
    }
    if (node->unsetsFlag != 0) {
        printf("Unsetting flag %u\n", node->unsetsFlag);
        set_flag(node->unsetsFlag, false);
        printf("Flags is now %lu\n", (unsigned long)progress.narrativeFlags);
    }
    printf("\"%s\"\n", node->text);
    size_t numOptions = 0;
    size_t accessibleNodes[maxOptions] = {};
    for (size_t i = 0; i < maxOptions; ++i) {
        // We don't have IDs on the options, cos we don't need them; 
        // so we just check if the option text is empty.
        if (node->options[i].text[0] == '\0') {
            break;
        }
        if (can_access_node(&convo->nodes[node->options[i].linksTo-1])) {
            accessibleNodes[numOptions] = i;
            ++numOptions;
        }
    }
    for (size_t i = 0; i < numOptions; ++i) {
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
        node = nullptr;
    } else {
        const unsigned link = node->options[accessibleNodes[selection]].linksTo;
        node = &(convo->nodes[link - 1]);
    }
    return node;
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

    const ConvoNode* node = &(convo->nodes[0]);
    size_t maxOptions = sizeof(node->options) / sizeof(node->options[0]);
    while (node != nullptr) {
        node = handle_node(convo, node, maxOptions);
    }
}

void handle_character(const size_t characterID) {
    const Person* const character = &allPeople[characterID - 1];
    printf("You are talking to %s\nTheir role is: %s\n\nDescription: %s\n\n\n", character->name, character->role, character->description);
    conversation(character->conversation);
}

/**
 * @brief Handles presenting and executing on options for the given room
 * @detail Note: modifies the `room` pointer, setting it to the next selected room, or nullptr if exit is selected.
 * @param room In/Out param, room to present, and overwritten with next selected room pointer, or nullptr if player chooses to exit.
 * @param maxConns Maximum number of connections from a room
 * @param maxChars Maximum number of characters a room can have
 */
Room* handle_room(Room* const room, const size_t maxConns, const size_t maxChars) {
    if (room->firstEntrySetsFlag != 0) {
        printf("Setting flag %u\n", room->firstEntrySetsFlag);
        set_flag(room->firstEntrySetsFlag, true);
        printf("Flags is now %lu\n", (unsigned long)progress.narrativeFlags);
    }
    if (room->firstEntryUnsetsFlag != 0) {
        printf("Unsetting flag %u\n", room->firstEntryUnsetsFlag);
        set_flag(room->firstEntryUnsetsFlag, false);
        printf("Flags is now %lu\n", (unsigned long)progress.narrativeFlags);
    }
    
    printf("You are in %s\n\t%s\n", room->name, room->description);
    
    size_t numConns = 0;
    size_t accessibleConns[maxConns] = {};
    for (size_t i = 0; i < maxConns; ++i) {
        const unsigned connectsTo = room->connections[i];
        if (connectsTo == 0) {
            break;
        }
        if (can_enter_room(connectsTo - 1)) {
            accessibleConns[numConns] = connectsTo;
            ++numConns;
        }
    }
    for (size_t i = 0; i < maxConns; ++i) {
        if (accessibleConns[i] == 0) {
            break;
        }
        printf("[%zu] Go to %s\n", i, siteFacility.rooms[accessibleConns[i]-1].name);
    }
    
    size_t numChars = 0;
    for (size_t i = 0; i < maxChars; ++i) {
        if (room->characters[i] == 0) {
            break;
        }
        printf("[%zu] Talk to %s\n", numConns+numChars, allPeople[room->characters[i]-1].name);
        ++numChars;
    }
    
    size_t totalOpts = numConns + numChars; // XXX Technically _could_ wrap around, but in practice won't.
    if (totalOpts == 0) {
        printf("No options available...\n");
        return nullptr;
    }
    printf("[%zu] Exit Game\n", totalOpts);

    int selection = -1;
    const int count = fscanf(stdin, "%d", &selection);
    if (count <= 0) {
        log_error("Could not read input!\n");
        return nullptr;
    }
    fscanf(stdin, "%*[^\n]");
    printf("Selection: %d\n", selection);
    if (selection < 0 || selection > totalOpts) {
        printf("Invalid Input!\n");
        return room;
    } else if (selection == totalOpts) {
        return nullptr;
    } else if (selection < numConns){
        const unsigned selectedRoomID = accessibleConns[selection];
        printf("Going to room %d\n", selectedRoomID);
        progress.currentRoom = selectedRoomID;
        return &(siteFacility.rooms[selectedRoomID-1]);
    } else {
        const int selectedPersonID = selection - numConns; // [1], [2] are rooms, [3] a convo; selection=3; numConns=2; 
        const unsigned charID = room->characters[selectedPersonID];
        printf("Time to chat to %s!\n", allPeople[charID-1].name);
        handle_character(charID);
        return room;
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
    size_t maxChars = sizeof(room->characters) / sizeof(room->characters[0]);
    while (!exit) {
        room = handle_room(room, maxConns, maxChars);
        if (room == nullptr) {
            break;
        }
    }
}


void play_game() {
    traverse_rooms(progress.currentRoom);
}
