#include "organisation.h"

int main() {
    Organisation org = {
        .budget = 20,
        .reputation = 10,
        .mtfs = {0},
        .scps = {0},
        .playerName = "hello"
    };
    print_organisation_details(&org);
}
