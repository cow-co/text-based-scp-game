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

void inflict_casualties(Personnel* const personnel, const uint16_t dead, const uint16_t wounded);
void regenerate(Personnel* const personnel, const uint16_t count, const bool fromWounded);

typedef struct {
  const unsigned id;
  Personnel personnel;
  bool active;
  const char[64] name;
} MTF;


typedef struct {
    unsigned budget;
    uint8_t reputation;
    unsigned[32] scps;   // IDs of contained SCPs
    const char[64] playerName;
    MTF* mtfs;
} Organisation;

void print_organisation_details(const Organisation* const org);
void parse_organisation_data(const char* const filePath, const Organisation* outOrg);
void change_reputation(Organisation* const org, const int16_t delta);
#endif
