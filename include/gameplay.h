#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stddef.h>
#include <limits.h>
#include "organisation.h"

typedef struct {
    unsigned budgetScaling;
    uint8_t scpRepScaling;
    uint8_t researchRepScaling;
    uint8_t repCostOnBreachPerWeek;
    unsigned costPerMTFMem;
    unsigned costPerWoundedMem;
    double mtfWoundingChance;
    double mtfDeathChance;
    double scpCaptureChance;
    unsigned repairBaseCost;
    double baseRecontainProb;
} GameplayConfig;
extern GameplayConfig gameplayConfig;
int parse_gameplay_config(const char* const filePath);

/**
 * @brief Sets/unsets the given flag
 * @details NOTE: if `flagBit` is zero, does nothing (does not error)
 * @param flagBit the (1-based) flag number to set/unset. If zero, does nothing.
 * @param set whether to set or unset the given flag
 */
void set_flag(size_t flagBit, const bool set);

/**
 * @brief Checks the given flag
 * @details NOTE: if `flagBit` is zero, returns false
 * @param flagBit the (1-based) flag number to check. If zero, returns false.
 * @returns bool whether the flag is set
 */
bool test_flag(size_t flagBit);

typedef struct {
    size_t setsFlag;        // Optional
    size_t requiresFlag;    // Optional (though this or requiresWeek is required)
    uint8_t requiresWeek;   // Optional (though this or requiresFlag is required)
    int budgetDelta;        // Optional
    size_t newMTFAssigned;  // Optional
    char text[8192];        // Required
} Event;
extern Event events[16];
int parse_events(const char* const filePath);

typedef struct {
    size_t currentRoom;
    _BitInt(64) narrativeFlags;
    Organisation org;
    bool gameOver;
    uint8_t week;   // Success criterion is reaching week 52 (one year) so this should be more than big enough
} Progress;

extern char playerName[64];
extern Progress progress;
void conversation(const size_t id);
void traverse_rooms(const size_t fromRoomID);
void play_game();

#endif
