#include "organisation.h"
#include "characters.h"
#include "conversation.h"
#include "menu.h"
#include "gameplay.h"
#include "map.h"
#include <stdio.h>
#include <math.h>
#include "util.h"

int main() {
    Progress progress = {
        .currentRoom = 0,
        .narrativeFlags = 0,
        .org = {
            .budget = 0,
            .reputation = 0,
            .mtfs = {},
            .scps = {},
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
    printf("////////// Finished Parsing Data Files //////////\n");
    present_menu();
}
