#include "organisation.h"
#include "tomlc17.h"
#include "util.h"
#include <stdio.h>
#include <tgmath.h>


MTF allMTFs[32] = {};


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

int parse_mtf_data(const char* const filePath) {
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not parse MTFs config file!\n");
        return EOF;
    }
    const toml_datum_t mtfsDatum = toml_seek(tomlResult.toptab, "mtf");

    if (mtfsDatum.type != TOML_ARRAY) {
        log_error("The mtf field in the MTFs config file must be an array!\n");
        return EOF;
    }

    if (mtfsDatum.u.arr.size > sizeof(allMTFs)/sizeof(allMTFs[0])) {
        log_error("There are too many MTFs in the MTF config file!\n");
        return EOF;
    }

    for (size_t i = 0; i < mtfsDatum.u.arr.size; ++i) {
        if (mtfsDatum.u.arr.elem[i].type != TOML_TABLE) {
            log_error("Each item in the mtf array in the MTFs config file must be a table!\n");
            return EOF;
        }

        // TODO parse the MTF into an MTF object and add to the allMTFs array 
        // XXX Is it worth keeping a separate global for the number of MTFs (and similar for SCPs)? To prevent having to recalculate each time we need to traverse those arrays?
    }

    return 0;
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
        log_error("Could not parse organisation config file!\n");
        return EOF;
    }

    const toml_datum_t repDatum = toml_seek(tomlResult.toptab, "organisation.reputation");
    const toml_datum_t budDatum = toml_seek(tomlResult.toptab, "organisation.budget");
    const toml_datum_t skipsDatum = toml_seek(tomlResult.toptab, "organisation.scps");
    const toml_datum_t mtfsDatum = toml_seek(tomlResult.toptab, "organisation.mtfs");
    if (repDatum.type != TOML_INT64) {
        log_error("In the organisation TOML, reputation should be an integer!");
        return EOF;
    }
    if (budDatum.type != TOML_INT64) {                                                                                                        
        log_error("In the organisation TOML, budget should be an integer!");
        return EOF;
    }
    if (skipsDatum.type != TOML_ARRAY) {                                                                                                        
        log_error("In the organisation TOML, scps should be an array!");
        return EOF;
    }
    if (mtfsDatum.type != TOML_ARRAY) {                                                                                                        
        log_error("In the organisation TOML, mtfs should be an array!");
        return EOF;
    }

    // TODO Verify it's also less than or equal to 100
    if (repDatum.u.int64 < 0 || repDatum.u.int64 > 100) {
        log_error("In the organisation TOML, reputation should be 0 <= rep <= 100!");
        return EOF;
    }
    if (budDatum.u.int64 < 0) {                                                                                                        
        log_error("In the organisation TOML, budget should be non-negative!");
        return EOF;
    }

    // XXX This only works if outOrg->scps is initialised with values; which I think is a reasonable contract.
    if (skipsDatum.u.arr.size > sizeof(outOrg->scps) / sizeof(outOrg->scps[0])) {
        log_error("In the organisation TOML, scps array is too long!");
        return EOF;
    }
    // XXX This only works if outOrg->mtfs is initialised with values; which I think is a reasonable contract.                                                                          
    if (mtfsDatum.u.arr.size > sizeof(outOrg->mtfs) / sizeof(outOrg->mtfs[0])) {                                                                                                        
        log_error("In the organisation TOML, mtfs array is too long!");                                                                                                       
        return EOF;                                                                                                                                                                     
    }
    
    outOrg->reputation = repDatum.u.int64;
    outOrg->budget = budDatum.u.int64;

    for (size_t i = 0; i < skipsDatum.u.arr.size; ++i) {
        const toml_datum_t skipIDDatum = skipsDatum.u.arr.elem[i];
        // 0 holds special meaning as an end-marker in our ID arrays, 
        // so don't allow actual IDs to have that value
        if (skipIDDatum.type != TOML_INT64 || skipIDDatum.u.int64 <= 0) {                                                                                                        
            log_error("In the organisation TOML, the scps array elements should be strictly positive integers!");
            return EOF;
        }
        outOrg->scps[i] = skipIDDatum.u.int64;
    }

    for (size_t i = 0; i < mtfsDatum.u.arr.size; ++i) {                                                                                                                                
        const toml_datum_t mtfIDDatum = mtfsDatum.u.arr.elem[i];                                                                                                                      
        // 0 holds special meaning as an end-marker in our ID arrays,                                                                                                                   
        // so don't allow actual IDs to have that value                                                                                                                                 
        if (mtfIDDatum.type != TOML_INT64 || mtfIDDatum.u.int64 <= 0) {
            log_error("In the organisation TOML, the mtfs array elements should be strictly positive integers!");
            return EOF;                                                                                                                                                                 
        }                                                                                                                                                                               
        outOrg->mtfs[i] = mtfIDDatum.u.int64;                                                                                                                                          
    }

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
