#include "organisation.h"
#include "narrative.h"
#include <stdio.h>
#include <math.h>

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
    unsigned long expected = pow(2, 10);
    printf("%d (should print %lu)\n", flags, expected);
}
