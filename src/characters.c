#include "characters.h"
#include "util.h"
#include "tomlc17.h"
#include <stdio.h>
#include <string.h>


SCP allSCPs[32] = {};
Conversation allConvos[512] = {};

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
    printf("SCPs:\n");
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

int parse_convos(const char* const filePath) {

}

void conversation(const unsigned id) {
    Conversation* convo = nullptr;
    size_t countConvos = sizeof(allConvos) / sizeof(allConvos[0]);
    for (size_t i = 0; i < countConvos; ++i) {
        if (allConvos[i].id == id) {
            convo = &allConvos[i];
            break;
        } else if (allConvos[i].id == 0) {
            break;
        }
    }
    if (convo == nullptr) {
        log_error("Conversation not found!\n");
        return;
    }
    printf("%s\n", convo->text);
    // TODO List options
    // TODO Allow selection of an option
    // TODO Move to selected convo node
    bool exit = false;
    while (!exit) {
        // TODO Make sure to always insert an "exit" option
    }
}

int parse_people(const char* const filePath) {
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not parse persons file!\n");
        return EOF;
    }

    const toml_datum_t personsDatum = toml_get(tomlResult.toptab, "person");
    if (personsDatum.type != TOML_ARRAY) {
        log_error("Persons list must be an array!\n");
        return EOF;
    }

    // TODO Parse array
    return 0;
}
