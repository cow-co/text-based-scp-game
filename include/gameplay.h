#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stddef.h>
#include <limits.h>
#include "organisation.h"

void set_flag(size_t flagBit, const bool set);
bool test_flag(size_t flagBit);

typedef struct {
    size_t currentRoom;
    _BitInt(64) narrativeFlags;
    Organisation org;
} Progress;

extern char playerName[64];
extern Progress progress;
void play_game();

#endif
