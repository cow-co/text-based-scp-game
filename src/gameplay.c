#include "gameplay.h"
#include "organisation.h"
#include "map.h"
#include "characters.h"
#include "conversation.h"
#include <stdio.h>


char playerName[64] = {};
_BitInt(64) flags = 0;
Progress progress = {};

void set_flag(size_t flagBit, const bool set) {
    if (flagBit > 64) {
        flagBit = 64;
    }
    flags = flags | (set << flagBit);
}

bool test_flag(size_t flagBit) {
    if (flagBit > 64) {
        flagBit = 64;
    }
    bool set = flags & (1 << flagBit);
    return set;
}

void play_game() {
    traverse_rooms(1);
}
