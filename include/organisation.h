#include <stdint.h>
#include "constraints.h"

enum Reputation {
    LOW = 0;
    MODERATE = 50;
    HIGH = 100;
};

typedef struct {
  operational: uint16_t;
  wounded: uint16_t;
  dead: uint16_t;
} Personnel;

typedef struct {
  id: unsigned;
  name: char[];
  personnel: Personnel;
  active: bool;
} MTF;


typedef struct {
    budget: unsigned;
    reputation: unsigned;
    mtfs: MTF[];
    scps: unsigned[];   // IDs of contained SCPs
    playerName: char[];
} Organisation;

void print_organisation_details(const Organisation* const org);
