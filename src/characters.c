#include "characters.h"
#include "util.h"
#include "tomlc17.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


SCP allSCPs[32] = {};
Person allPeople[32] = {};


int parse_scps(const char* const filePath) {
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not parse SCPs file!\n");
        return EOF;
    }
    
    const toml_datum_t scpsDatum = toml_get(tomlResult.toptab, "scp");
    
    if (scpsDatum.type != TOML_ARRAY) {
        log_error("SCPs list should be an array!");
        return EOF;
    }
    
    if (scpsDatum.u.arr.size > sizeof(allSCPs) / sizeof(allSCPs[0])) {
        log_error("SCPs list is too long!\n");
        return EOF;
    }

    for (size_t i = 0; i < scpsDatum.u.arr.size; ++i) {
        const toml_datum_t scpDatum = scpsDatum.u.arr.elem[i];
        if (scpDatum.type != TOML_TABLE) {
            log_error("SCP array elements should be tables!\n");
            return EOF;
        }
        const toml_datum_t scpIDDatum = toml_get(scpDatum, "id");
        const toml_datum_t scpNameDatum = toml_get(scpDatum, "name");
        const toml_datum_t scpDescriptionDatum = toml_get(scpDatum, "description");
        const toml_datum_t scpObjClassDatum = toml_get(scpDatum, "objectClass");
        
        if (scpIDDatum.type != TOML_INT64 || scpIDDatum.u.int64 <= 0) {
            log_error("SCP IDs must be strictly positive integers!\n");
            return EOF;
        }
        if (scpNameDatum.type != TOML_STRING) {
            log_error("SCP names must be strings!\n");
            return EOF;
        }
        if (scpDescriptionDatum.type != TOML_STRING) {
            log_error("SCP descriptions must be strings!\n");
            return EOF;
        }
        if (scpObjClassDatum.type != TOML_INT64 || scpObjClassDatum.u.int64 < SAFE || scpObjClassDatum.u.int64 > KETER) {
            log_error("SCP Object class should be an integer corresponding to one of the object class enum constants!\n");
            return EOF;
        }

        if (scpNameDatum.u.str.len >= sizeof(allSCPs[i].name)) {
            log_error("SCP Name is too long!\n");
            return EOF;
        }
        if (scpDescriptionDatum.u.str.len >= sizeof(allSCPs[i].description)) {
            log_error("SCP description is too long!\n");
            return EOF;
        }

        allSCPs[i].id = scpIDDatum.u.int64;
        strncpy(allSCPs[i].name, scpNameDatum.u.str.ptr, scpNameDatum.u.str.len);
        strncpy(allSCPs[i].description, scpDescriptionDatum.u.str.ptr, scpDescriptionDatum.u.str.len);
        allSCPs[i].objectClass = scpObjClassDatum.u.int64;
    }
    return 0;
}

void print_scps() {
    size_t countSCPs = sizeof(allSCPs) / sizeof(allSCPs[0]);
    for (size_t i = 0; i < countSCPs; ++i) {
        char* class = "SAFE";
        if (allSCPs[i].id == 0) {
            return;
        }
        if (allSCPs[i].objectClass == EUCLID) {
            class = "EUCLID";
        } else if (allSCPs[i].objectClass == KETER) {
            class = "KETER";
        }
        printf(
            "%s\n\tID: %u\n\tObject Class: %s\n\tDescription: %s\n", 
            allSCPs[i].name,
            allSCPs[i].id,
            class,
            allSCPs[i].description
        );
    }
}


int parse_people(const char* const filePath) {
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not parse persons file!\n");
        return EOF;
    }

    const toml_datum_t personsDatum = toml_get(tomlResult.toptab, "person");
    if (personsDatum.type != TOML_ARRAY || personsDatum.u.arr.elem[0].type != TOML_TABLE) {
        log_error("Persons list must be an array of tables!\n");
        return EOF;
    }
    if (personsDatum.u.arr.size > sizeof(allPeople)/sizeof(allPeople[0])) {
        log_error("Too many elements in Persons list!\n");
        return EOF;
    }

    for (size_t i = 0; i < personsDatum.u.arr.size; ++i) {
        const toml_datum_t personIDDatum = toml_get(personsDatum.u.arr.elem[i], "id");
        const toml_datum_t personNameDatum = toml_get(personsDatum.u.arr.elem[i], "name");
        const toml_datum_t personRoleDatum = toml_get(personsDatum.u.arr.elem[i], "role");
        const toml_datum_t personDescDatum = toml_get(personsDatum.u.arr.elem[i], "description");
        const toml_datum_t personConvosDatum = toml_get(personsDatum.u.arr.elem[i], "conversations");

        if (personIDDatum.type != TOML_INT64 || personIDDatum.u.int64 <= 0) {
            log_error("Person ID must be a strictly positive (> 0) integer!\n");
            return EOF;
        }
        if (personNameDatum.type != TOML_STRING || personNameDatum.u.str.len >= sizeof(allPeople[0].name) || personNameDatum.u.str.len == 0) {
            log_error("Person name is not a string or is too long or has no content!\n");
            return EOF;
        }
        if (personRoleDatum.type != TOML_UNKNOWN && (personRoleDatum.type != TOML_STRING || personRoleDatum.u.str.len >= sizeof(allPeople[0].role))) {
            log_error("Person role is not a string or is too long!");
            return EOF;
        }
        if (personRoleDatum.type != TOML_UNKNOWN && (personRoleDatum.type != TOML_STRING || personRoleDatum.u.str.len >= sizeof(allPeople[0].role))) {
            log_error("Person description is not a string or is too long!");
            return EOF;
        }
        if (personConvosDatum.type != TOML_UNKNOWN && (personConvosDatum.type != TOML_ARRAY || personConvosDatum.u.arr.elem[0].type != TOML_INT64)) {
            log_error("Person conversations must be an array of integers!\n");
            return EOF;
        }

        allPeople[i].id = personIDDatum.u.int64;
        strncpy(allPeople[i].name, personNameDatum.u.str.ptr, personNameDatum.u.str.len);
        if (personRoleDatum.u.str.len > 0) {
            strncpy(allPeople[i].role, personRoleDatum.u.str.ptr, personRoleDatum.u.str.len);
        }
        if (personDescDatum.u.str.len > 0) {
            strncpy(allPeople[i].description, personDescDatum.u.str.ptr, personDescDatum.u.str.len);
        }

        for (size_t j = 0; j < personConvosDatum.u.arr.size; ++j) {
            allPeople[i].conversations[j] = personConvosDatum.u.arr.elem[j].u.int64;
        }
    }

    return 0;
}


void print_people() {
    size_t maxPeople = sizeof(allPeople) / sizeof(allPeople[0]);
    for (size_t i = 0; i < maxPeople; ++i) {
        if (allPeople[i].id == 0) {
            break;
        }
        printf("%s\n\tID: %u\n\tRole: %s\n\tDescription: %s\n", allPeople[i].name, allPeople[i].id, allPeople[i].role, allPeople[i].description);
    }
}
