#include "organisation.h"
#include <stdio.h>
#include <tgmath.h>


/**
 * Inflicts the given numbers of casualties on the given personnel entity.
 * Ensures that the number of active personnel does not wrap around
 * if losses exceed the number of personnel remaining.
 */
void inflict_casualties(Personnel* const personnel, const uint16_t dead, const uint16_t wounded) {
    uint16_t newOperational = personnel->operational;
    if (dead >= newOperational) {
        newOperational = 0;
    } else {
        newOperational -= dead;
    }
    personnel->dead += dead;    // TODO Prevent wraparound
    
    if (wounded >= newOperational) {
        newOperational = 0;
    } else {
        newOperational -= wounded;
    }
    personnel->wounded += wounded;  // TODO Prevent wraparound 
}

/**
 * Regenerates active personnel into the personnel pool
 */
void regenerate(Personnel* const personnel, const uint16_t count, const bool fromWounded) {
    uint16_t toAdd = count;
    if (fromWounded && count >= personnel->wounded) {
        toAdd = personnel->wounded;
        personnel->wounded = 0;
    }
    personnel->operational += toAdd;    // TODO Prevent Wraparound
}

/**
 * Displays all the information about an organisation;
 * currently mostly for debug purposes, but may later be extended to 
 * be used for actual gameplay.
 */
void print_organisation_details(const Organisation* const org) {
    printf("Organisation details: \n \
            Budget: %u \n \
            Reputation: %u \n \
            Player name: \n", org->budget, org->reputation);
}

/**
 * Reads in a data file and fills the data into an Organisation object
 */
void parse_organisation_data(char* fileName, const Organisation* outOrg) {
    // TODO Fill this out
}


/**
 * Updates changes the reputation of the organisation by the given amount.
 * Note: maximum is 100 rep, so this will calculate the floor.
 */
void change_reputation(Organisation* const org, const int16_t delta) {
    if (-org->reputation > delta) {
        org->reputation = 0;
    } else if (org->reputation+delta > 100) {
        org->reputation = 100;
    } else {
        org->reputation = org->reputation + delta;
    }
}
