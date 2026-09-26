#include "organisation.h"
#include "narrative.h"
#include <stdio.h>

int main() {
    Organisation org = {
        .budget = 20,
        .reputation = 10,
        .mtfs = nullptr,
        .scps = nullptr,
        .playerName = "hello"
    };
    print_organisation_details(&org);
    set_flag(10, true);
    printf("%d\n", flags);
}
