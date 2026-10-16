#include "gameplay.h"
#include "organisation.h"
#include "map.h"
#include "characters.h"
#include "conversation.h"
#include "util.h"
#include "counts.h"
#include "tomlc17.h"
#include <stdio.h>
#include <stdlib.h>


char playerName[64] = {};
Progress progress = {};
GameplayConfig gameplayConfig = {};

int parse_gameplay_config(const char* const filePath) {
    const toml_result_t tomlResult= toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not read gameplay config file!\n");
        printf("%s\n", tomlResult.errmsg);
        toml_free(tomlResult);
        return EOF;
    }

    const toml_datum_t budgetScalingDatum = toml_get(tomlResult.toptab, "budgetScaling");
    const toml_datum_t scpRepScalingDatum = toml_get(tomlResult.toptab, "scpRepScaling");
    const toml_datum_t researchRepScalingDatum = toml_get(tomlResult.toptab, "researchRepScaling");
    const toml_datum_t repCostOnBreachPerWeekDatum = toml_get(tomlResult.toptab, "repCostOnBreachPerWeek");
    const toml_datum_t costPerMTFMemDatum = toml_get(tomlResult.toptab, "costPerMTFMem");
    const toml_datum_t costPerWoundedMemDatum = toml_get(tomlResult.toptab, "costPerWoundedMem");
    const toml_datum_t mtfWoundingChanceDatum = toml_get(tomlResult.toptab, "mtfWoundingChance");
    const toml_datum_t mtfDeathChanceDatum = toml_get(tomlResult.toptab, "mtfDeathChance");
    const toml_datum_t scpCaptureChanceDatum = toml_get(tomlResult.toptab, "scpCaptureChance");
    const toml_datum_t repairBaseCostDatum = toml_get(tomlResult.toptab, "repairBaseCost");
    
    if (budgetScalingDatum.type != TOML_INT64 || budgetScalingDatum.u.int64 < 0) {
        log_error("Budget scaling must be an integer >= 0!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (scpRepScalingDatum.type != TOML_INT64 || scpRepScalingDatum.u.int64 < 0 || scpRepScalingDatum.u.int64 > 100) {
        log_error("SCP Rep scaling must be an integer >= 0 and <= 100!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (researchRepScalingDatum.type != TOML_INT64 || researchRepScalingDatum.u.int64 < 0 || researchRepScalingDatum.u.int64 > 100) {
        log_error("Research Rep scaling must be an integer >= 0 and <= 100!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (repCostOnBreachPerWeekDatum.type != TOML_INT64 || repCostOnBreachPerWeekDatum.u.int64 < 0 || repCostOnBreachPerWeekDatum.u.int64 > 100) {
        log_error("Rep cost on breach must be an integer >= 0 and <= 100!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (costPerMTFMemDatum.type != TOML_INT64 || costPerMTFMemDatum.u.int64 < 0) {
        log_error("Cost per MTF member must be an integer >= 0!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (costPerWoundedMemDatum.type != TOML_INT64 || costPerWoundedMemDatum.u.int64 < 0) {
        log_error("Cost per wounded MTF member must be an integer >= 0!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (mtfWoundingChanceDatum.type != TOML_FP64 || mtfWoundingChanceDatum.u.fp64 < 0) {
        log_error("Wounding chance must be a float >= 0!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (mtfDeathChanceDatum.type != TOML_FP64 || mtfDeathChanceDatum.u.fp64 < 0) {
        log_error("Death chance must be a float >= 0!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (scpCaptureChanceDatum.type != TOML_FP64 || scpCaptureChanceDatum.u.fp64 < 0) {
        log_error("Capture chance must be a float >= 0!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (repairBaseCostDatum.type != TOML_INT64 || repairBaseCostDatum.u.int64 < 0) {
        log_error("Repair cost must be an integer >= 0!\n");
        toml_free(tomlResult);
        return EOF;
    }

    gameplayConfig.budgetScaling = budgetScalingDatum.u.int64;
    gameplayConfig.scpRepScaling = scpRepScalingDatum.u.int64;
    gameplayConfig.researchRepScaling = researchRepScalingDatum.u.int64;
    gameplayConfig.repCostOnBreachPerWeek = repCostOnBreachPerWeekDatum.u.int64;
    gameplayConfig.costPerMTFMem = costPerMTFMemDatum.u.int64;
    gameplayConfig.costPerWoundedMem = costPerWoundedMemDatum.u.int64;
    gameplayConfig.mtfWoundingChance = costPerWoundedMemDatum.u.fp64;
    gameplayConfig.mtfDeathChance = costPerWoundedMemDatum.u.fp64;
    gameplayConfig.scpCaptureChance = scpCaptureChanceDatum.u.fp64;
    gameplayConfig.repairBaseCost = repairBaseCostDatum.u.int64;

    toml_free(tomlResult);
    return 0;
}

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
        printf("Which SCP's containment should we repair?\n");
        size_t total = 0;
        unsigned breachedSCPs[countSCPs] = {};
        for (size_t i = 0; i < countSCPs; ++i) {
            // XXX We could pre-calculate this at load-time for each SCP and store in the SCP struct
            unsigned cost = gameplayConfig.repairBaseCost * allSCPs[i].objectClass;
            if (
                allSCPs[i].breached 
                && cost >= progress.org.budget.totalAmount
            ) {
                printf(
                    "[%zu] %s (Costs %u)\n", 
                    total, 
                    allSCPs[i].name, 
                    cost
                );
                breachedSCPs[total] = i;
                ++total;
            }
        }
        printf("[%zu] Back", total);

        int selection = read_numeric_input(total, 0);

        if (selection == total) {
            return;
        } else if (selection != -1) {
            allSCPs[breachedSCPs[selection]].breached = false;
            change_budget(&progress.org, gameplayConfig.repairBaseCost * allSCPs[selection].objectClass);
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

        int selection = read_numeric_input(4, 0);

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

void adjust_budget() {
    const int baseIncome = progress.org.reputation * gameplayConfig.budgetScaling;
    const int totalUnallocated = ((100 - progress.org.budget.containmentAlloc - progress.org.budget.researchAlloc) / 100);
    const int remainingAfterAllocations = baseIncome * totalUnallocated;
    
    unsigned numOp = 0;
    unsigned numWounded = 0;
    for (size_t i = 0; i < sizeof(progress.org.mtfs) / sizeof(progress.org.mtfs[0]); ++i) {
        if (progress.org.mtfs[i] == 0) {
            break;
        }
        const unsigned mtfID = progress.org.mtfs[i];
        numOp += allMTFs[mtfID-1].personnel.operational;
        numWounded += allMTFs[mtfID-1].personnel.wounded;
    }
    const unsigned mtfCost = (numOp * gameplayConfig.costPerMTFMem) + (numWounded * gameplayConfig.costPerWoundedMem);

    const int totalDelta = remainingAfterAllocations - mtfCost;
    change_budget(&progress.org, totalDelta);
}

void adjust_reputation() {
    int16_t repDelta = 0;
    unsigned numSCPsContained = 0;
    unsigned numBreaches = 0;
    
    for (size_t i = 0; i < countSCPs; ++i) {
        if (allSCPs[i].contained) {
            ++numSCPsContained;
        }
        if (allSCPs[i].breached) {
            ++numBreaches;
        }
    }

    repDelta = (numSCPsContained * gameplayConfig.scpRepScaling) 
                + (progress.org.budget.researchAlloc * gameplayConfig.researchRepScaling) 
                - (numBreaches * gameplayConfig.repCostOnBreachPerWeek);
    change_reputation(&progress.org, repDelta);
}

unsigned calculate_scp_capture() {
    unsigned scpID = 0;

    // Did we capture an SCP?
    int randNum = (rand() / RAND_MAX / 100);    // Percentage
    int chancePercent = (int)(gameplayConfig.scpCaptureChance * 100);
    unsigned chances[countSCPs] = {}; // This is a VLA, yes, but the size is determined at compile-time, so it's pretty safe.
    if (randNum <= chancePercent) {
        // Which SCPs are eligible for capture?
        for (size_t i = 0; i < countSCPs; ++i) {
            unsigned prev = 0;
            if (i > 0) {
                prev = chances[i-1];
            }
            if (allSCPs[i].contained) {
                chances[i] = prev;
            } else {
                chances[i] = (int)(allSCPs[i].captureProb * 100) + prev;
            }
        }
        // We now have a max for our PRNG (will need to *100  and cast to int though...)
        // Now we calculate the rand number, and see which SCP's "probability bucket" it falls into
        int scpSelectionRand = (rand() / RAND_MAX / chances[countSCPs-1]);
        for (size_t i = 0; i < countSCPs; ++i) {
            if (scpSelectionRand <= chances[i]) {
                scpID = i + 1;
                break;
            }
        }
    }
    return scpID;
}

/**
 * @brief Runs the RNG to check if any of the returning MTFs have contained a new SCP
 * @details Ensure you run this BEFORE adjusting the `deploymentWeeksRemaining`, since this assumes that `deploymentWeeksRemaining=1` means the 
 *          MTF is returning _this_ week. If you decrement `deploymentWeeksRemaining` and unset `deployed` before this, the MTF won't be picked up.
 */
void contain_scps() {
    for (size_t i = 0; i < sizeof(progress.org.mtfs) / sizeof(progress.org.mtfs[0]); ++i) {
        const unsigned mtfID = progress.org.mtfs[i];
        if (mtfID == 0) {
            break;
        }
        const MTF* mtf = &allMTFs[mtfID-1];
        if (mtf->deployed && mtf->deploymentWeeksRemaining == 1) {
            unsigned cappedSCP = calculate_scp_capture();
            if (cappedSCP > 0) {
                allSCPs[cappedSCP-1].contained = true;
                // TODO Add to budget
            }
            // TODO MTF losses on SCP-capture
        }
    }
}

void present_win_message() {
    clear_screen();
    printf("//////////////////////////////////////////\n");
    printf("Congratulations, %s, you won the game by reaching week 52 (a whole year)!\n", playerName);
    printf("//////////////////////////////////////////\n");
}

void present_loser_message() {
    clear_screen();
    printf("//////////////////////////////////////////\n");
    printf("Unfortunately, %s, you lost the game by reaching 0 reputation or 0 funds...\n", playerName);
    printf("//////////////////////////////////////////\n");
}

void advance_time() {
    const size_t maxMTFsInOrg = sizeof(progress.org.mtfs) / sizeof(progress.org.mtfs[0]);
    for (size_t i = 0; i < maxMTFsInOrg; ++i) {
        if (progress.org.mtfs[i] == 0) {
            break;
        } else {
            MTF* const mtf = &allMTFs[progress.org.mtfs[i]-1];
            if (mtf->deployed) {
                --mtf->deploymentWeeksRemaining;
                if (!mtf->deploymentWeeksRemaining) {
                    mtf->deployed = false;
                    printf("%s has returned from deployment!\n", mtf->name);
                }
            }
        }
    }
    adjust_reputation();
    if (progress.week % 4 == 0) {
        adjust_budget();
    }
    ++progress.week;
}

void present_containment_inspection() {
    // TODO FILL OUT
}

Room* present_high_level_options(Room* room) {
    printf("You are in %s\n\t%s\n", room->name, room->description);
    print_organisation_details(&progress.org);
    printf("[0] Go to another room\n");
    printf("[1] Talk to people\n");
    printf("[2] Interact with items\n");
    printf("[3] Manage organisation\n");
    printf("[4] Advance time by a week\n");
    printf("[5] Inspect Containment\n");
    printf("[6] Exit\n");

    int selection = read_numeric_input(5, 0);

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
            advance_time();
            return room;
        case 5:
            present_containment_inspection();
            return room;
        case 6:
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
        if (progress.week == 52) {
            present_win_message();
            break;
        } else if (progress.org.budget.totalAmount == 0 || progress.org.reputation == 0) {
            present_loser_message();
            break;
        } else if (room == nullptr) {
            break;
        }
    }
}


void play_game() {
    traverse_rooms(progress.currentRoom);
}
