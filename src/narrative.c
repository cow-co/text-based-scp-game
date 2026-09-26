#include "narrative.h"

_BitInt(64) flags = 0;

void set_flag(size_t flagBit, const bool set) {
    if (flagBit > 64) {
        flagBit = 64;
    }
    flags = flags | (set << flagBit);
}
