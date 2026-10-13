#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stddef.h>
#include <limits.h>
#include "organisation.h"

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
    size_t currentRoom;
    _BitInt(64) narrativeFlags;
    Organisation org;
    uint8_t week;   // Success criterion is reaching week 52 (one year) so this should be more than big enough
} Progress;

extern char playerName[64];
extern Progress progress;
void conversation(const size_t id);
void traverse_rooms(const size_t fromRoomID);
void play_game();

#endif
