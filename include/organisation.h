#include <stdint.h>
#include "constants.h"

size_t const maxMTFs = 32;
size_t const maxOwnedSCPs = 64;

enum Reputation {
    LOW
    MODERATE
    HIGH
};

typedef struct {
  operational: uint16_t;
  wounded: uint16_t;
  dead: uint16_t;
} Personnel;

typedef struct {
  id: unsigned;
  name: char[nameMaxLen];
  personnel: Personnel;
  active: bool;
} MTF;


typedef struct {
    budget: unsigned;
    reputation: Reputation;
    mtfs: MTF[maxMTFs];
    scps: unsigned[maxOwnedSCPs];   // IDs of contained SCPs
    playerName: char[nameMaxLen];
} Organisation;
