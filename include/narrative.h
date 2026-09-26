#ifndef NARRATIVE_H
#define NARRATIVE_H

#include <stddef.h>
#include <limits.h>

extern _BitInt(64) flags;

void set_flag(size_t flagBit, const bool set);

#endif
