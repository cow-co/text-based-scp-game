#include "organisation.h"
#include "narrative.h"
#include "characters.h"
#include "conversation.h"
#include "map.h"
#include <stdio.h>
#include <math.h>
#include "util.h"

int main() {
    Organisation org = {
        .budget = 0,
        .reputation = 0,
        .mtfs = {},
        .scps = {},
        .playerName = ""
    };
    printf("////////// Parsing Data Files //////////\n");
    if (parse_scps("data/scps.toml") == EOF) {
        return 1;
    }
    if (parse_mtf_data("data/mtfs.toml") == EOF) {
        return 1;
    }
    if (parse_organisation_data("data/organisation.toml", &org) == EOF) {
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
    printf("////////// People //////////\n");
    print_people();
    printf("////////// SCPs //////////\n");
    print_scps();
    printf("////////// MTFs //////////\n");
    print_mtfs();
    printf("////////// Facility //////////\n");
    print_facility();
    traverse_rooms(1);
    conversation(1);
}
