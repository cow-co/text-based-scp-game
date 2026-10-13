#include "gameplay.h"
#include "organisation.h"
#include "map.h"
#include "characters.h"
#include "conversation.h"
#include "util.h"
#include "counts.h"
#include <stdio.h>


char playerName[64] = {};
Progress progress = {};

void set_flag(size_t flagBit, const bool set) {
    if (flagBit > 64) {
        flagBit = 64;
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

bool can_access_interactable(const Interactable* const interactable) {
    if (interactable->requiresFlag || interactable->unrequiresFlag) {
        return (test_flag(interactable->requiresFlag) && !test_flag(interactable->unrequiresFlag));
    } else {
        return true;
    }
}

const ConvoNode* handle_node(const Conversation* const convo, const ConvoNode* node, size_t maxOptions) {
    // Using that set_flag() does nothing if flagBit is zero
    set_flag(node->setsFlag, true);
    set_flag(node->unsetsFlag, false);

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

    // Return to the top of the conversation tree if there are no more options available in this chain
    if (numOptions == 0) {
        return &(convo->nodes[0]);
    }

    for (size_t i = 0; i < numOptions; ++i) {
        printf("[%zu] %s\n", i, node->options[accessibleNodes[i]].text);
    }
    printf("[%zu] Back\n", numOptions);

    int selection = read_numeric_input(numOptions, 0);
    if (selection == numOptions) {
        node = nullptr;
    } else if (selection != -1) {
        const unsigned link = node->options[accessibleNodes[selection]].linksTo;
        node = &(convo->nodes[link - 1]);
    }
    return node;
}

void conversation(const size_t id) {
    Conversation* convo = nullptr;
    if (id <= countConvos) {
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
    clear_screen();
    const Person* const character = &allPeople[characterID - 1];
    printf("You are talking to %s\nTheir role is: %s\n\nDescription: %s\n\n\n", character->name, character->role, character->description);
    conversation(character->conversation);
}

void handle_interactions(const Room* const room) {
    clear_screen();
    printf("Here are the things you can interact with:\n");
    const size_t numInteractables = sizeof(room->interactables) / sizeof(room->interactables[0]);
    size_t numAccessible = 0;
    unsigned accessible[numInteractables] = {};
    for (size_t i = 0; i < numInteractables; ++i) {
        if (room->interactables[i] == 0) {
            break;
        }
        if (!can_access_interactable(&allInteractables[room->interactables[i]-1])) {
            continue;
        }
        printf(
            "%s [x%zu to examine] [i%zu to interact]\n", 
            allInteractables[room->interactables[i]-1].name, 
            numAccessible,
            numAccessible
        );
        accessible[i] = room->interactables[i];
        ++numAccessible;
    }
    if (numAccessible == 0) {
        printf("No options available...\n");
    }
    printf("[%zu] Back\n", numAccessible);

    bool exit = false;
    while (!exit) {
        // TODO Standardise on unsigned for IDs since we should never reach as high as would need a size_t (probably could do a like uint8 even...)
        char type;
        unsigned selection;
        const int count = fscanf(stdin, "%c%u", &type, &selection);
        if (count <= 0) {
            log_error("Could not read input!\n");
            return;
        }

        fscanf(stdin, "%*[^\n]");
        if (selection > numAccessible) {
            log_error("Invalid Input!\n");
        } else if (selection == numAccessible) {
            return;
        } else if (type == 'x') {
            printf("%s\n\n", allInteractables[accessible[selection]].inspectText);
        } else if (type == 'i') {
            printf("%s\n\n", allInteractables[accessible[selection]].interactText);
            set_flag(allInteractables[accessible[selection]].setsFlag, true);
            set_flag(allInteractables[accessible[selection]].unsetsFlag, false);
        } else {
            log_error("Invalid Input!\n");
        }
    }
}

uint8_t allocate_budget(uint8_t slackCapacity) {
    printf("What percentage of your budget do you wish to allocate? Your current allocations are:\n");
    printf("Containment: %u percent\n", progress.org.budget.containmentAlloc);
    printf("Research: %u percent\n", progress.org.budget.researchAlloc);
    printf("Total budget: %u\n", progress.org.budget.totalAmount);
    uint8_t selection = 0;

    while (true) {
        selection = read_numeric_input(slackCapacity, 0);

        if (selection != -1) {
            return selection;
        }
    }
}

void deploy_mtf() {
    printf("Select an MTF to deploy:\n\n");

    const size_t numMTFs = sizeof(progress.org.mtfs) / sizeof(progress.org.mtfs[0]);
    unsigned options = 0;
    unsigned assignable[numMTFs] = {};
    for (size_t i = 0; i < numMTFs; ++i) {
        const unsigned mtfID = progress.org.mtfs[i];
        if (mtfID == 0) {
            break;
        }

        const MTF* const mtf = &allMTFs[mtfID - 1];
        if (mtf->deployed || mtf->active || mtf->personnel.operational == 0) {
            continue;
        }
        assignable[options] = mtfID;
        printf(
            "[%zu] %s: %u operational, %u wounded, %u dead\n", 
            i, 
            mtf->name, 
            mtf->personnel.operational, 
            mtf->personnel.wounded, 
            mtf->personnel.dead
        );

        ++options;
    }
    printf("[%u] Back\n", options);

    unsigned selected = 0;
    unsigned selection = 0;
    while (true) {
        selection = read_numeric_input(options, 0);

        if (selection == options) {
            return;
        } else if (selection > -1) {
            selected = progress.org.mtfs[assignable[selection]];
        }
    }

    if (selected) {
        printf("Deploying MTF...\n");
        allMTFs[progress.org.mtfs[selected] - 1].deployed = true;
        allMTFs[progress.org.mtfs[selected] - 1].deploymentWeeksRemaining = 4;
    }
}

void repair_containment() {
    clear_screen();
    while (true) {
        printf("Which room should we repair?\n");
        size_t total = 0;
        unsigned brokenRooms[countRooms] = {};
        for (size_t i = 0; i < countRooms; ++i) {
            if (siteFacility.rooms[i].name[0] == '\0') {
                break;
            } else if (
                siteFacility.rooms[i].containmentBreached 
                && progress.org.budget.totalAmount >= siteFacility.rooms[i].repairCost
            ) {
                printf(
                    "[%zu] %s (Costs %u)\n", 
                    total, 
                    siteFacility.rooms[i].name, 
                    siteFacility.rooms[i].repairCost
                );
                brokenRooms[total] = i;
                ++total;
            }
        }
        printf("[%zu] Back", total);

        int selection = read_numeric_input(total, 0);

        if (selection == total) {
            return;
        } else if (selection != -1) {
            siteFacility.rooms[brokenRooms[selection]].containmentBreached = false;
            change_budget(&progress.org, siteFacility.rooms[brokenRooms[selection]].repairCost);
        }
    }
}

void handle_org_management() {
    clear_screen();
    bool exit = false;
    while (!exit) {
        printf("You can do the following:\n\n");
        printf("[0] Invest in better containment\n");
        printf("[1] Send an MTF to contain a new anomaly\n");
        printf("[2] Invest in more research\n");
        printf("[3] Repair broken containment rooms\n");
        printf("[4] Back\n");

        int selection = read_numeric_input(3, 0);

        switch (selection) {
            case -1:
                break;  // Error message shown by the input handler
            case 0:
                uint8_t newContainmentAlloc = allocate_budget(100 - progress.org.budget.researchAlloc);
                progress.org.budget.containmentAlloc = newContainmentAlloc;
                break;
            case 1:
                deploy_mtf();
                break;
            case 2:
                uint8_t newResearchAlloc = allocate_budget(100 - progress.org.budget.containmentAlloc);
                progress.org.budget.researchAlloc = newResearchAlloc;
                break;
            case 3:
                repair_containment();
                break;
            case 4:
                exit = true;
                break;
            default:
                log_error("Invalid input!\n");
                break;
        }
    }
}

/**
 * @brief Handles presenting and executing on options for the given room
 * @details Note: modifies the `room` pointer, setting it to the next selected room, or nullptr if exit is selected.
 * @param room room to present
 * @return Room* next room to go to, or `nullptr` if we want to quit
 */
Room* handle_room(Room* const room) {
    clear_screen();
    // Using that set_flag() does nothing if flagBit is zero
    // FIXME This'll run _every_ time we enter the room, not just the first time! Set a "hasBeenEntered" flag on the room (separate from narrative flags 
    // since another action might clobber the narrative flag later)
    set_flag(room->firstEntrySetsFlag, true);
    set_flag(room->firstEntryUnsetsFlag, false);
    
    size_t totalOpts = 0;
    size_t accessibleConns[maxConnections] = {};
    for (size_t i = 0; i < maxConnections; ++i) {
        const unsigned connectsTo = room->connections[i];
        if (connectsTo == 0) {
            break;
        }
        if (can_enter_room(connectsTo - 1)) {
            accessibleConns[totalOpts] = connectsTo;
            printf("[%zu] Go to %s\n", i, siteFacility.rooms[connectsTo-1].name);
            ++totalOpts;
        }
    }
    
    if (totalOpts == 0) {
        printf("No options available...\n");
        return nullptr;
    }
    printf("[%zu] Back\n", totalOpts);

    int selection = read_numeric_input(totalOpts, 0);

    if (selection == totalOpts) {
        return room;
    } else if (selection != -1) {
        return &siteFacility.rooms[accessibleConns[selection]-1];
    }

    return room;
}

void present_characters(const Room* const room) {
    while (true) {
        printf("Who do you want to speak to?\n");
        unsigned numOptions = 0;
        for (size_t i = 0; i < maxCharactersPerRoom; ++i) {
            if (room->characters[i] == 0) {
                break;
            }
            printf("[%zu] %s\n", i, allPeople[room->characters[i]-1].name);
            ++numOptions;
        }
        printf("[%u] Back\n", numOptions);
        int selection = read_numeric_input(numOptions, 0);
        if (selection == numOptions) {
            return;
        } else if (selection != -1) {
            handle_character(room->characters[selection]);
        }
    }
}

Room* present_high_level_options(Room* room) {
    printf("You are in %s\n\t%s\n", room->name, room->description);
    printf("[0] Go to another room\n");
    printf("[1] Talk to people\n");
    printf("[2] Interact with items\n");
    printf("[3] Manage organisation\n");
    printf("[4] Exit\n");

    int selection = read_numeric_input(4, 0);

    switch (selection) {
        case -1:
            break;  // Error message will already have been shown by the input handler
        case 0:
            return handle_room(room);
        case 1:
            present_characters(room);
            return room;
        case 2:
            handle_interactions(room);
            return room;
        case 3:
            handle_org_management();
            return room;
        case 4:
            return nullptr;
        default:
            log_error("Could not read input!\n");
            return room;
    }
    return room;
}

void traverse_rooms(const size_t fromRoomID) {
    Room* room = nullptr;
    if (fromRoomID <= countRooms) {
        room = &siteFacility.rooms[fromRoomID-1];
    } else {
        log_error("Room not found!\n");
        return;
    }

    bool exit = false;
    while (!exit) {
        room = present_high_level_options(room);
        if (room == nullptr) {
            break;
        }
    }
}


void play_game() {
    traverse_rooms(progress.currentRoom);
}
