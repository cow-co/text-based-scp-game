#include "organisation.h"
#include "narrative.h"
#include <stdio.h>
#include <math.h>

int main() {
    Organisation org = {
        .budget = 0,
        .reputation = 0,
        .mtfs = {},
        .scps = {},
        .playerName = ""
    };
    parse_organisation_data("data/organisation.toml", &org);
    print_organisation_details(&org);
}
