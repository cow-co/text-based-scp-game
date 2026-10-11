#include "map.h"
#include "tomlc17.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>


Facility siteFacility = {};
Interactable allInteractables[64] = {};

int parse_room(
    const toml_datum_t* const parent, 
    Room* const room, 
    const size_t maxNameLength, 
    const size_t maxDescLength, 
    const size_t maxConnections,
    const size_t maxCharacters
) {
    const toml_datum_t roomNameDatum = toml_get(*parent, "name");
    const toml_datum_t roomDescriptionDatum = toml_get(*parent, "description");
    const toml_datum_t roomConnectionsDatum = toml_get(*parent, "connections");
    const toml_datum_t roomSetsDatum = toml_get(*parent, "firstEntrySetsFlag");
    const toml_datum_t roomUnsetsDatum = toml_get(*parent, "firstEntryUnsetsFlag");
    const toml_datum_t roomRequiresSetDatum = toml_get(*parent, "flagRequired");
    const toml_datum_t roomRequiresUnsetDatum = toml_get(*parent, "disabledFlag");
    const toml_datum_t roomCharactersDatum = toml_get(*parent, "characters");
    const toml_datum_t roomSCPDatum = toml_get(*parent, "containedSCP");

    if (roomNameDatum.type != TOML_STRING || roomNameDatum.u.str.len >= maxNameLength || roomNameDatum.u.str.len == 0) {
        log_error("Room name is not a string, or is too long, or has no content!\n");
        return EOF;            
    }
    if (roomDescriptionDatum.type != TOML_STRING || roomDescriptionDatum.u.str.len >= maxDescLength) {
        log_error("Room description is not a string, or is too long!\n");
        return EOF;
    }
    if (roomSetsDatum.type != TOML_UNKNOWN && (roomSetsDatum.type != TOML_INT64 || roomSetsDatum.u.int64 <= 0)) {
        log_error("Room sets must be integer > 0!\n");
        return EOF;
    }
    if (roomUnsetsDatum.type != TOML_UNKNOWN && (roomUnsetsDatum.type != TOML_INT64 || roomUnsetsDatum.u.int64 <= 0)) {
        log_error("Room unsets must be integer > 0!\n");
        return EOF;
    }
    if (roomRequiresSetDatum.type != TOML_UNKNOWN && (roomRequiresSetDatum.type != TOML_INT64 || roomRequiresSetDatum.u.int64 <= 0)) {
        log_error("Room flag required must be integer > 0!\n");
        return EOF;
    }
    if (roomRequiresUnsetDatum.type != TOML_UNKNOWN && (roomRequiresUnsetDatum.type != TOML_INT64 || roomRequiresUnsetDatum.u.int64 <= 0)) {
        log_error("Room disabled flag must be integer > 0!\n");
        return EOF;
    }
    if (roomConnectionsDatum.type != TOML_ARRAY || roomConnectionsDatum.u.arr.size >= maxConnections || roomConnectionsDatum.u.arr.elem[0].type != TOML_INT64) {
        log_error("Room connections is not an array of integers, or is too long!\n");
        return EOF;
    }
    if (roomCharactersDatum.type != TOML_UNKNOWN 
        && (roomCharactersDatum.type != TOML_ARRAY 
            || roomCharactersDatum.u.arr.size >= maxCharacters 
            || roomCharactersDatum.u.arr.elem[0].type != TOML_INT64)
    ) {
        log_error("Room characters is not an array of integers, or is too long!\n");
        return EOF;
    }
    if (roomSCPDatum.type != TOML_UNKNOWN && (roomSCPDatum.type != TOML_INT64 || roomSCPDatum.u.int64 <= 0)) {
        log_error("Room SCP must be an integer > 0!\n");
        return EOF;
    }

    strncpy(room->name, roomNameDatum.u.str.ptr, roomNameDatum.u.str.len);
    strncpy(room->description, roomDescriptionDatum.u.str.ptr, roomDescriptionDatum.u.str.len);
    if (roomSCPDatum.type != TOML_UNKNOWN) {
        room->containedSCP = roomSCPDatum.u.int64;
    }
    if (roomSetsDatum.type != TOML_UNKNOWN) {
        room->firstEntrySetsFlag = roomSetsDatum.u.int64;
    }
    if (roomUnsetsDatum.type != TOML_UNKNOWN) {
        room->firstEntryUnsetsFlag = roomUnsetsDatum.u.int64;
    }
    if (roomRequiresSetDatum.type != TOML_UNKNOWN) {
        room->requiresFlagSet = roomRequiresSetDatum.u.int64;
    }
    if (roomRequiresUnsetDatum.type != TOML_UNKNOWN) {
        room->requiresFlagUnset = roomRequiresUnsetDatum.u.int64;
    }
    if (roomCharactersDatum.type != TOML_UNKNOWN) {
        for (size_t j = 0; j < roomCharactersDatum.u.arr.size; ++j) {
            if (roomCharactersDatum.u.arr.elem[j].u.int64 <= 0) {
                log_error("Character ID must be an integer > 0!\n");
                return EOF;
            }
            room->characters[j] = roomCharactersDatum.u.arr.elem[j].u.int64;
        }
    }
    for (size_t j = 0; j < roomConnectionsDatum.u.arr.size; ++j) {
        if (roomConnectionsDatum.u.arr.elem[j].u.int64 <= 0) {
            log_error("Room ID must be an integer > 0!\n");
            return EOF;
        }
        room->connections[j] = roomConnectionsDatum.u.arr.elem[j].u.int64;
    }

    return 0;
}

int parse_facility(const char* const filePath) {
    clear_array(sizeof(siteFacility), &siteFacility);
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not parse facility file!\n");
        toml_free(tomlResult);
        return EOF;
    }
    const toml_datum_t entrancesDatum = toml_get(tomlResult.toptab, "entrances");
    if (entrancesDatum.type != TOML_ARRAY || entrancesDatum.u.arr.elem[0].type != TOML_INT64) {
        log_error("Entrances list must be an array of integers!\n");
        toml_free(tomlResult);
        return EOF;
    }
    size_t maxEntrances = sizeof(siteFacility.entranceRooms) / sizeof(siteFacility.entranceRooms[0]);
    if (entrancesDatum.u.arr.size > maxEntrances) {
        log_error("Entrances list is too long!\n");
        toml_free(tomlResult);
        return EOF;
    }
    for (size_t i = 0; i < entrancesDatum.u.arr.size; ++i) {
        siteFacility.entranceRooms[i] = entrancesDatum.u.arr.elem[i].u.int64;
    }

    const toml_datum_t roomsDatum = toml_get(tomlResult.toptab, "room");
    if (roomsDatum.type != TOML_ARRAY || roomsDatum.u.arr.elem[0].type != TOML_TABLE) {
        log_error("Rooms list must be an array of tables!\n");
        toml_free(tomlResult);
        return EOF;
    }
    size_t maxRooms = sizeof(siteFacility.rooms) / sizeof(siteFacility.rooms[0]);
    if (roomsDatum.u.arr.size > maxRooms) {
        log_error("Rooms list is too long!\n");
        toml_free(tomlResult);
        return EOF;
    }

    const size_t maxNameLength = sizeof(siteFacility.rooms[0].name);
    const size_t maxDescLength = sizeof(siteFacility.rooms[0].description);
    const size_t maxConnections = sizeof(siteFacility.rooms[0].connections) / sizeof(siteFacility.rooms[0].connections[0]);
    const size_t maxCharacters = sizeof(siteFacility.rooms[0].characters) / sizeof(siteFacility.rooms[0].characters[0]);
    for (size_t i = 0; i < roomsDatum.u.arr.size; ++i) {
        if (parse_room(
            &roomsDatum.u.arr.elem[i], 
            &siteFacility.rooms[i], 
            maxNameLength, 
            maxDescLength, 
            maxConnections, 
            maxCharacters) 
            == EOF
        ) {
            toml_free(tomlResult);
            return EOF;    
        }
    }

    toml_free(tomlResult);
    return 0;
}

void print_facility() {
    const size_t numRooms = sizeof(siteFacility.rooms) / sizeof(siteFacility.rooms[0]);
    for (size_t i = 0; i < numRooms; ++i) {
        if (siteFacility.rooms[i].name[0] == '\0') {
            break;
        }
        printf("%s\n\t%s\n\tContains: %u\n", siteFacility.rooms[i].name, siteFacility.rooms[i].description, siteFacility.rooms[i].containedSCP);
    }
}

int parse_interactable(Interactable* const interactable, const toml_datum_t* const parent) {
    const toml_datum_t nameDatum = toml_get(*parent, "name");
    const toml_datum_t inspectDatum = toml_get(*parent, "inspectText");
    const toml_datum_t interactDatum = toml_get(*parent, "interactText");
    const toml_datum_t requiredDatum = toml_get(*parent, "requiresFlag");
    const toml_datum_t unrequiredDatum = toml_get(*parent, "unrequiresFlag");
    const toml_datum_t setsDatum = toml_get(*parent, "setsFlag");
    const toml_datum_t unsetsDatum = toml_get(*parent, "unsetsFlag");

    const size_t maxNameLen = sizeof(interactable->name);
    const size_t maxInteractLen = sizeof(interactable->interactText);
    const size_t maxInspectLen = sizeof(interactable->inspectText);

    if (nameDatum.type != TOML_STRING || nameDatum.u.str.len >= maxNameLen) {
        log_error("Interactable name is not a string, or is too long!\n");
        return EOF;
    }
    if (inspectDatum.type != TOML_STRING || inspectDatum.u.str.len >= maxInspectLen) {
        log_error("Interactable inspect-text is not a string, or is too long!\n");
        return EOF;
    }
    if (interactDatum.type != TOML_STRING || interactDatum.u.str.len >= maxInteractLen) {
        log_error("Interactable interact-text is not a string, or is too long!\n");
        return EOF;
    }
    if (requiredDatum.type != TOML_UNKNOWN && (requiredDatum.type != TOML_INT64 || requiredDatum.u.int64 <= 0)){
        log_error("Interactable required flag must be an integer > 0!\n");
        return EOF;
    }
    if (unrequiredDatum.type != TOML_UNKNOWN && (unrequiredDatum.type != TOML_INT64 || unrequiredDatum.u.int64 <= 0)) {
        log_error("Interactable unrequired flag must be an integer > 0!\n");
        return EOF;
    }
    if (setsDatum.type != TOML_UNKNOWN && (setsDatum.type != TOML_INT64 || setsDatum.u.int64 <= 0)) {
        log_error("Interactable set flag must be an integer > 0!\n");
        return EOF;
    }
    if (unsetsDatum.type != TOML_UNKNOWN && (unsetsDatum.type != TOML_INT64 || unsetsDatum.u.int64 <= 0)) {
        log_error("Interactable unset flag must be an integer > 0!\n");
        return EOF;
    }
    
    strncpy(interactable->name, nameDatum.u.str.ptr, nameDatum.u.str.len);
    strncpy(interactable->interactText, interactDatum.u.str.ptr, interactDatum.u.str.len);
    strncpy(interactable->inspectText, inspectDatum.u.str.ptr, inspectDatum.u.str.len);

    if (requiredDatum.type != TOML_UNKNOWN) {
        interactable->requiresFlag = requiredDatum.u.int64;
    }
    if (unrequiredDatum.type != TOML_UNKNOWN) {
        interactable->unrequiresFlag = unrequiredDatum.u.int64;
    }
    if (setsDatum.type != TOML_UNKNOWN) {
        interactable->setsFlag = setsDatum.u.int64;
    }
    if (unsetsDatum.type != TOML_UNKNOWN) {
        interactable->unsetsFlag = unsetsDatum.u.int64;
    }

    return 0;
}

int parse_interactables(const char* const filePath) {
    clear_array(sizeof(allInteractables), &allInteractables);
    const toml_result_t tomlResult = toml_parse_file_ex(filePath);
    if (!tomlResult.ok) {
        log_error("Could not parse interactables file!\n");
        toml_free(tomlResult);
        return EOF;
    }

    const toml_datum_t interactablesDatum = toml_get(tomlResult.toptab, "interactable");
    const size_t maxInteractables = sizeof(allInteractables) / sizeof(Interactable);
    if (interactablesDatum.type != TOML_ARRAY || interactablesDatum.u.arr.elem[0].type != TOML_TABLE || interactablesDatum.u.arr.size > maxInteractables) {
        log_error("Interactables should be an array of tables; or it is too long!\n");
        toml_free(tomlResult);
        return EOF;
    }

    for (size_t i = 0; i < interactablesDatum.u.arr.size; ++i) {
        if (parse_interactable(&allInteractables[i], &interactablesDatum.u.arr.elem[i]) == EOF) {
            toml_free(tomlResult);
            return EOF;
        }
    }

    toml_free(tomlResult);
    return 0;
}
