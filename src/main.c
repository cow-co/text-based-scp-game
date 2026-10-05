#include "organisation.h"
#include "narrative.h"
#include "characters.h"
#include "conversation.h"
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
    parse_scps("data/scps.toml");
    parse_mtf_data("data/mtfs.toml");
    parse_organisation_data("data/organisation.toml", &org);
    parse_convos("data/convos.toml");
    parse_people("data/people.toml");
    printf("////////// Finished Parsing Data Files //////////\n");
    printf("////////// People //////////\n");
    print_people();
    printf("////////// SCPs //////////\n");
    print_scps();
    printf("////////// MTFs //////////\n");
    print_mtfs();
    conversation(1);
}
