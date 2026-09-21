#include "organisation.h"

int main() {
    Organisation org = {
        .budget = 20,
        .reputation = 10,
        .mtfs = nullptr,
        .scps = nullptr,
        .playerName = "hello"
    };
    print_organisation_details(&org);
}
