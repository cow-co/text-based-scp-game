#include <stdint.h>
#include "constraints.h"

enum Reputation {
    LOW = 0;
    MODERATE = 50;
    HIGH = 100;
};

typedef struct {
  uint16_t operational;
  uint16_t wounded;
  uint16_t dead;
} Personnel;

typedef struct {
  unsigned id;
  char name[];
  Personnel personnel;
  bool active;
} MTF;


typedef struct {
    unsigned budget;
    unsigned reputation;
    MTF mtfs[];
    unsigned scps[];   // IDs of contained SCPs
    char playerName[];
} Organisation;

void print_organisation_details(const Organisation* const org);
