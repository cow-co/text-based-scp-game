#include "organisation.h"
#include "tomlc17.h"
#include <stdio.h>
#include <tgmath.h>


allMTFs = {};


void inflict_casualties(Personnel* const personnel, const uint16_t dead, const uint16_t wounded) {
    uint16_t newOperational = personnel->operational;
    if (dead >= newOperational) {
        newOperational = 0;
    } else {
        newOperational -= dead;
    }
    personnel->dead += dead;    // TODO Prevent wraparound
    
    if (wounded >= newOperational) {
        newOperational = 0;
    } else {
        newOperational -= wounded;
    }
    personnel->wounded += wounded;  // TODO Prevent wraparound 
}

void regenerate(Personnel* const personnel, const uint16_t count, const bool fromWounded) {
    uint16_t toAdd = count;
    if (fromWounded && count >= personnel->wounded) {
        toAdd = personnel->wounded;
        personnel->wounded = 0;
    }
    personnel->operational += toAdd;    // TODO Prevent Wraparound
}

void print_organisation_details(const Organisation* const org) {
    printf("Organisation details: \n \
            Budget: %u \n \
            Reputation: %u \n \
            Player name: \n", org->budget, org->reputation);
}

int parse_organisation_data(const char* const filePath, Organisation* const outOrg) {
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        // TODO invoke an error message util
        return EOF;
    }

    const toml_datum_t repDatum = toml_seek(tomlResult.toptab, "organisation.reputation");
    const toml_datum_t budDatum = toml_seek(tomlResult.toptab, "organisation.budget");
    const toml_datum_t skipsDatum = toml_seek(tomlResult.toptab, "organisation.scps");
    if (repDatum.type != TOML_INT64) {
        // TODO invoke error util
        return EOF;
    }
    if (budDatum.type != TOML_INT64) {
        // TODO Invoke error util
        return EOF;
    }
    if (skipsDatum.type != TOML_ARRAY) {
        // TODO Invoke error util
        return EOF;
    }

    // TODO Verify it's also less than or equal to 100
    if (repDatum.u.int64 < 0) {
        // TODO Invoke error util
        return EOF;
    }
    if (budDatum.u.int64 < 0) {
        // TODO Invoke error util
        return EOF;
    }
    // TODO Check skip array sizes versus size of skip list in Org struct
    
    outOrg->reputation = repDatum.u.int64;
    outOrg->budget = budDatum.u.int64;
    outOrg->scps = skipDatum.u.elem;

    // TODO Load all MTFs
    // TODO SCPs
    // TODO Player-controlled MTFs

    toml_free(tomlResult);
    return 0;
}


void change_reputation(Organisation* const org, const int16_t delta) {
    if (-org->reputation > delta) {
        org->reputation = 0;
    } else if (org->reputation+delta > 100) {
        org->reputation = 100;
    } else {
        org->reputation = org->reputation + delta;
    }
}
