#ifndef ORGANISATION_H
#define ORGANISATION_H

#include <stddef.h>
#include <stdint.h>
#include "constraints.h"

enum Reputation {
    LOW = 0,
    MODERATE = 50,
    HIGH = 100
};

typedef struct {
  uint16_t operational;
  uint16_t wounded;
  uint16_t dead;
} Personnel;

typedef struct {
  unsigned id;
  Personnel personnel;
  bool active;
  char* name;
} MTF;


typedef struct {
    unsigned budget;
    unsigned reputation;
    unsigned* scps;   // IDs of contained SCPs
    char* playerName;
    MTF* mtfs;
} Organisation;

void print_organisation_details(const Organisation* const org);

#endif
