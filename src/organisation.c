#include "organisation.h"
#include <stdio.h>


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
