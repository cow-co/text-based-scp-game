#include "organisation.h"
#include "characters.h"
#include "conversation.h"
#include "menu.h"
#include "gameplay.h"
#include "map.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main() {
    // Some gameplay features require PRNG, so let's seed that
    // Technically time() needn't return an integer type, but in practice all reasonable implementations do.
    // See https://en.cppreference.com/c/numeric/random/srand
    srand(time(NULL));

    Progress progress = {
        .currentRoom = 0,
        .narrativeFlags = 0,
        .org = {
            .budget = {},
            .reputation = 0,
            .mtfs = {},
        },
    };
    printf("////////// Parsing Data Files //////////\n");
    if (parse_scps("data/scps.toml") == EOF) {
        return 1;
    }
    if (parse_mtf_data("data/mtfs.toml") == EOF) {
        return 1;
    }
    if (parse_organisation_data("data/organisation.toml", &progress.org) == EOF) {
        return 1;
    }
    if (parse_convos("data/convos.toml") == EOF) {
        return 1;
    }
    if (parse_people("data/people.toml") == EOF) {
        return 1;
    }
    if (parse_facility("data/map.toml") == EOF) {
        return 1;
    }
    if (parse_gameplay_config("data/gameplay.toml") == EOF) {
        return 1;
    }
    printf("////////// Finished Parsing Data Files //////////\n");
    present_menu();
}
