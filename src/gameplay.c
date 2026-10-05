#include "gameplay.h"
#include "organisation.h"
#include "narrative.h"
#include "map.h"
#include "characters.h"
#include "conversation.h"
#include <stdio.h>


char playerName[64] = {};


void play_game() {
    traverse_rooms(1);
}
