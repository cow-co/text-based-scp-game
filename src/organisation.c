#include "organisation.h"
#include "tomlc17.h"
#include "util.h"
#include <stdio.h>
#include <tgmath.h>
#include <string.h>
#include <limits.h>


MTF allMTFs[32] = {};


void inflict_casualties(Personnel* const personnel, const uint16_t dead, const uint16_t wounded) {
    uint16_t newOperational = personnel->operational;
    if (dead >= newOperational) {
        personnel->dead += newOperational;
        newOperational = 0;
    } else {
        newOperational -= dead;
        personnel->dead += dead;    // TODO Prevent wraparound
    }
    
    if (wounded >= newOperational) {
        personnel->wounded += newOperational;
        newOperational = 0;
    } else {
        newOperational -= wounded;
        personnel->wounded += wounded;
    }
    personnel->operational = newOperational;
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
    clear_array(sizeof(allMTFs), &allMTFs);
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not parse MTFs config file!\n");
        toml_free(tomlResult);
        return EOF;
    }
    const toml_datum_t mtfsDatum = toml_seek(tomlResult.toptab, "mtf");

    if (mtfsDatum.type != TOML_ARRAY) {
        log_error("The mtf field in the MTFs config file must be an array!\n");
        toml_free(tomlResult);
        return EOF;
    }

    if (mtfsDatum.u.arr.size > sizeof(allMTFs)/sizeof(allMTFs[0])) {
        log_error("There are too many MTFs in the MTF config file!\n");
        toml_free(tomlResult);
        return EOF;
    }

    for (size_t i = 0; i < mtfsDatum.u.arr.size; ++i) {
        const toml_datum_t mtfDatum = mtfsDatum.u.arr.elem[i];
        if (mtfDatum.type != TOML_TABLE) {
            log_error("Each item in the mtf array in the MTFs config file must be a table!\n");
            toml_free(tomlResult);
            return EOF;
        }
        
        const toml_datum_t mtfName = toml_get(mtfDatum, "name");
        if (mtfName.type != TOML_STRING) {
            log_error("MTF Name must be a string!\n");
            toml_free(tomlResult);
            return EOF;
        }
        if (mtfName.u.str.len >= sizeof(allMTFs[i].name)) {
            log_error("MTF Name is too long!\n");
            toml_free(tomlResult);
            return EOF;
        }
        
        const toml_datum_t mtfPers = toml_get(mtfDatum, "personnel");
        if (mtfPers.type != TOML_INT64) {
            log_error("MTF personnel must be an integer!\n");
            toml_free(tomlResult);
            return EOF;
        }
        if (mtfPers.u.int64 < 0) {
            log_error("MTF personnel must be non-negative!\n");
            toml_free(tomlResult);
            return EOF;
        }

        Personnel pers = {
            .wounded = 0,
            .dead = 0,
            .operational = mtfPers.u.int64
        };
        allMTFs[i].personnel = pers;
        // We strncpy this because the pointer to the TOML string is invalidated
        // when we call toml_free()
        strncpy(allMTFs[i].name, mtfName.u.str.ptr, mtfName.u.str.len);
    }

    toml_free(tomlResult);
    return 0;
}

void print_mtfs() {
    size_t countMTFs = sizeof(allMTFs) / sizeof(allMTFs[0]);
    for (size_t i = 0; i < countMTFs; ++i) {
        if (allMTFs[i].name[0] == '\0') {
            return;
        }
        printf(
            "%s\n\tOperational: %u\n\tWounded: %u\n\tDead: %u\n", 
            allMTFs[i].name, 
            allMTFs[i].personnel.operational, 
            allMTFs[i].personnel.wounded, 
            allMTFs[i].personnel.dead
        );
    }
}

void print_organisation_details(const Organisation* const org) {
    printf("Organisation details: \n \
            Budget: %u \n \
            Reputation: %u \n", 
            org->budget.totalAmount, 
            org->reputation
    );
}

int parse_organisation_data(const char* const filePath, Organisation* const outOrg) {
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not parse organisation config file!\n");
        toml_free(tomlResult);
        return EOF;
    }

    const toml_datum_t repDatum = toml_seek(tomlResult.toptab, "organisation.reputation");
    const toml_datum_t budDatum = toml_seek(tomlResult.toptab, "organisation.budget");
    const toml_datum_t skipsDatum = toml_seek(tomlResult.toptab, "organisation.scps");
    const toml_datum_t mtfsDatum = toml_seek(tomlResult.toptab, "organisation.mtfs");
    if (repDatum.type != TOML_INT64) {
        log_error("In the organisation TOML, reputation should be an integer!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (budDatum.type != TOML_INT64) {                                                                                                        
        log_error("In the organisation TOML, budget should be an integer!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (skipsDatum.type != TOML_ARRAY) {                                                                                                        
        log_error("In the organisation TOML, scps should be an array!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (mtfsDatum.type != TOML_ARRAY) {                                                                                                        
        log_error("In the organisation TOML, mtfs should be an array!\n");
        toml_free(tomlResult);
        return EOF;
    }

    if (repDatum.u.int64 < 0 || repDatum.u.int64 > 100) {
        log_error("In the organisation TOML, reputation should be 0 <= rep <= 100!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (budDatum.u.int64 < 0) {                                                                                                        
        log_error("In the organisation TOML, budget should be non-negative!\n");
        toml_free(tomlResult);
        return EOF;
    }

    if (skipsDatum.u.arr.size > sizeof(outOrg->scps) / sizeof(outOrg->scps[0])) {
        log_error("In the organisation TOML, scps array is too long!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (mtfsDatum.u.arr.size > sizeof(outOrg->mtfs) / sizeof(outOrg->mtfs[0])) {                                                                                                        
        log_error("In the organisation TOML, mtfs array is too long!\n");
        toml_free(tomlResult);
        return EOF;                                                                                                                                                                     
    }
    
    outOrg->reputation = repDatum.u.int64;
    outOrg->budget.totalAmount = budDatum.u.int64;

    for (size_t i = 0; i < skipsDatum.u.arr.size; ++i) {
        const toml_datum_t skipIDDatum = skipsDatum.u.arr.elem[i];
        if (skipIDDatum.type != TOML_INT64 || skipIDDatum.u.int64 <= 0) {                                                                                                        
            log_error("In the organisation TOML, the scps array elements should be strictly positive integers!\n");
            toml_free(tomlResult);
            return EOF;
        }
        outOrg->scps[i] = skipIDDatum.u.int64;
    }

    for (size_t i = 0; i < mtfsDatum.u.arr.size; ++i) {                                                                                                                                
        const toml_datum_t mtfIDDatum = mtfsDatum.u.arr.elem[i];                                                                                                                      
        if (mtfIDDatum.type != TOML_INT64 || mtfIDDatum.u.int64 <= 0) {
            log_error("In the organisation TOML, the mtfs array elements should be strictly positive integers!\n");
            toml_free(tomlResult);
            return EOF;                                                                                                                                                                 
        }                                                                                                                                                                               
        outOrg->mtfs[i] = mtfIDDatum.u.int64;                                                                                                                                          
    }

    toml_free(tomlResult);
    return 0;
}

void change_reputation(Organisation* const org, const int16_t delta) {
    if (delta < 0 && -delta > org->reputation) {
        org->reputation = 0;
    } else if (org->reputation+delta > 100) {
        org->reputation = 100;
    } else {
        org->reputation = org->reputation + delta;
    }
}

void change_budget(Organisation* const org, const int16_t delta) {
    if (delta < 0 && -delta > org->budget.totalAmount) {
        org->budget.totalAmount = 0;
    } else if (delta > 0 && (UINT_MAX - org->budget.totalAmount) < delta) {
        // Protect against wraparound
        org->budget.totalAmount = UINT_MAX;
    } else {
        org->budget.totalAmount = org->budget.totalAmount + delta;
    }
}