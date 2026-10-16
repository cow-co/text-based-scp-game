#include "menu.h"
#include "gameplay.h"
#include "util.h"
#include "tomlc17.h"
#include "map.h"
#include "characters.h"
#include "counts.h"
#include <stdio.h>
#include <string.h>


const char saveFileName[] = "data/save.toml";


int save_game(const char* const path) {
    FILE* file = fopen(path, "w");

    if (!file) {
        log_error("Could not open file for writing!\n");
        return EOF;
    }

    fprintf(file, "narrative_flags = %ld\n", (long)(progress.narrativeFlags));
    fprintf(file, "player_name = \"%s\"\n", playerName);
    fprintf(file, "current_room = %zu\n", progress.currentRoom);
    fprintf(file, "week = %hhu\n", progress.week);
    
    fprintf(file, "[organisation]\n");
    fprintf(file, "reputation = %u\n", progress.org.reputation);
    fprintf(file, "contQuality = %u\n", progress.org.containmentQuality);
    fprintf(file, "budget = %u\n", progress.org.budget.totalAmount);
    fprintf(file, "containAlloc = %u\n", progress.org.budget.containmentAlloc);
    fprintf(file, "researchAlloc = %u\n", progress.org.budget.researchAlloc);
    fprintf(file, "scps = [");
    
    bool isStart = true;
    for (size_t i = 0; i < countSCPs; ++i) {
        if(allSCPs[i].contained) {
            if (!isStart) {
                fprintf(file, ",");
            } else {
                isStart = false;
            }
            fprintf(file, "%zu", i+1);
        } 
    }
    fprintf(file, "]\n");
    fprintf(file, "breachedSCPs = [");
    
    isStart = true;
    for (size_t i = 0; i < countSCPs; ++i) {
        if(allSCPs[i].breached) {
            if (!isStart) {
                fprintf(file, ",");
            } else {
                isStart = false;
            }

            fprintf(file, "%zu", i+1);
        } 
    }
    fprintf(file, "]\n");

    
    const size_t countMTFs = sizeof(progress.org.mtfs) / sizeof(progress.org.mtfs[0]);
    for (size_t i = 0; i < countMTFs; ++i) {
        if (progress.org.mtfs[i]) {
            const unsigned id = progress.org.mtfs[i];
            fprintf(file, "[[mtf]]\n");
            fprintf(file, "id = %u\n", id);
            fprintf(file, "active = %s\n", allMTFs[i].active ? "true" : "false");
            fprintf(file, "deployed = %s\n", allMTFs[i].deployed ? "true" : "false");
            fprintf(file, "deploymentWeeksRemaining = %u\n", allMTFs[i].deploymentWeeksRemaining);
            fprintf(file, "operational = %d\n", allMTFs[id].personnel.operational);
            fprintf(file, "wounded = %d\n", allMTFs[id].personnel.wounded);
            fprintf(file, "dead = %d\n", allMTFs[id].personnel.dead);
        }
    }
    fclose(file);
    return 0;
}


int load_game(const char* const path) {
    printf("Loading game from %s\n", path);

    const toml_result_t tomlResult= toml_parse_file_ex(path);
    if (!tomlResult.ok) {
        log_error("Could not read saved-game file!\n");
        printf("%s\n", tomlResult.errmsg);
        toml_free(tomlResult);
        return EOF;
    }
    
    const toml_datum_t nameDatum = toml_get(tomlResult.toptab, "player_name");
    const toml_datum_t roomDatum = toml_get(tomlResult.toptab, "current_room");
    const toml_datum_t weekDatum = toml_get(tomlResult.toptab, "week");
    const toml_datum_t narrativeDatum = toml_get(tomlResult.toptab, "narrative_flags");
    const toml_datum_t orgDatum = toml_get(tomlResult.toptab, "organisation");

    const size_t maxNameLen = sizeof(playerName);
    if (nameDatum.type != TOML_STRING || nameDatum.u.str.len >= maxNameLen) {
        log_error("Name field is not a string, or is too long!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (roomDatum.type != TOML_INT64) {
        log_error("Room field is not an integer!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (weekDatum.type != TOML_INT64) {
        log_error("Week field is not an integer!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (narrativeDatum.type != TOML_INT64) {
        log_error("Narrative flags field is not an integer!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (orgDatum.type != TOML_TABLE) {
        log_error("Organisation field is not a table!\n");
        toml_free(tomlResult);
        return EOF;
    }

    strncpy(playerName, nameDatum.u.str.ptr, nameDatum.u.str.len);
    progress.currentRoom = roomDatum.u.int64;
    progress.week = weekDatum.u.int64;
    progress.narrativeFlags = (_BitInt(64))narrativeDatum.u.int64;

    const size_t countMTFs = sizeof(progress.org.mtfs) / sizeof(progress.org.mtfs[0]);

    const toml_datum_t repDatum = toml_get(orgDatum, "reputation");
    const toml_datum_t budDatum = toml_get(orgDatum, "budget");
    const toml_datum_t resDatum = toml_get(orgDatum, "researchAlloc");
    const toml_datum_t conDatum = toml_get(orgDatum, "containAlloc");
    const toml_datum_t scpDatum = toml_get(orgDatum, "scps");
    const toml_datum_t breachDatum = toml_get(orgDatum, "breachedSCPs");
    const toml_datum_t mtfDatum = toml_get(orgDatum, "mtf");
    if (repDatum.type != TOML_INT64 || repDatum.u.int64 < 0) {
        log_error("Reputation must be an integer >= 0!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (budDatum.type != TOML_INT64 || budDatum.u.int64 < 0) {
        log_error("Budget must be an integer >= 0!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (resDatum.type != TOML_INT64 || resDatum.u.int64 < 0 || resDatum.u.int64 > 100) {
        log_error("Research allocation must be an integer >= 0 and <= 100!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (conDatum.type != TOML_INT64 || conDatum.u.int64 < 0 || conDatum.u.int64 > 100) {
        log_error("Containment allocation must be an integer >= 0 and <= 100!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (scpDatum.type != TOML_ARRAY || (scpDatum.u.arr.size != 0 && scpDatum.u.arr.elem[0].type != TOML_INT64)) {
        log_error("SCPs must be an array of integers (or empty)!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (scpDatum.u.arr.size > countSCPs) {
        log_error("Contained SCPs array is too long!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (breachDatum.type != TOML_ARRAY || (breachDatum.u.arr.size != 0 && breachDatum.u.arr.elem[0].type != TOML_INT64)) {
        log_error("Breached rooms must be an array of integers (or empty)!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (breachDatum.u.arr.size > countSCPs) {
        log_error("Breached SCPs array is too long!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (mtfDatum.type != TOML_UNKNOWN && (mtfDatum.type != TOML_ARRAY || mtfDatum.u.arr.elem[0].type != TOML_TABLE)) {
        log_error("MTFs must be an array of tables!\n");
        toml_free(tomlResult);
        return EOF;
    }
    if (mtfDatum.u.arr.size > countMTFs) {
        log_error("MTFs array is too long!\n");
        toml_free(tomlResult);
        return EOF;
    }
    
    progress.org.reputation = repDatum.u.int64;
    progress.org.budget.totalAmount = budDatum.u.int64;
    progress.org.budget.containmentAlloc = conDatum.u.int64;
    progress.org.budget.researchAlloc = resDatum.u.int64;
    if (scpDatum.u.arr.size > 0) {
        for (size_t i = 0; i < scpDatum.u.arr.size; ++i) {
            allSCPs[scpDatum.u.arr.elem[i].u.int64-1].contained = true;
        }
    }
    if (breachDatum.u.arr.size > 0) {
        for (size_t i = 0; i < breachDatum.u.arr.size; ++i) {
            allSCPs[breachDatum.u.arr.elem[i].u.int64-1].breached = true;
        }
    }
    if (mtfDatum.type != TOML_UNKNOWN && mtfDatum.u.arr.size > 0) {
        for (size_t i = 0; i < mtfDatum.u.arr.size; ++i) {
            const toml_datum_t mtfIDDatum = toml_get(mtfDatum.u.arr.elem[i], "id");
            const toml_datum_t mtfActiveDatum = toml_get(mtfDatum.u.arr.elem[i], "active");
            const toml_datum_t mtfDeployedDatum = toml_get(mtfDatum.u.arr.elem[i], "deployed");
            const toml_datum_t mtfWeeksRemDatum = toml_get(mtfDatum.u.arr.elem[i], "deploymentWeeksRemaining");
            const toml_datum_t mtfOperationalDatum = toml_get(mtfDatum.u.arr.elem[i], "operational");
            const toml_datum_t mtfWoundedDatum = toml_get(mtfDatum.u.arr.elem[i], "wounded");
            const toml_datum_t mtfDeadDatum = toml_get(mtfDatum.u.arr.elem[i], "dead");
            if (mtfIDDatum.type != TOML_INT64 || mtfIDDatum.u.int64 <= 0) {
                log_error("MTF ID Must be an integer > 0!\n");
                toml_free(tomlResult);
                return EOF;
            }
            if (mtfActiveDatum.type != TOML_BOOLEAN) {
                log_error("MTF active must be a boolean!\n");
                toml_free(tomlResult);
                return EOF;
            }
            if (mtfDeployedDatum.type != TOML_BOOLEAN) {
                log_error("MTF deployed must be a boolean!\n");
                toml_free(tomlResult);
                return EOF;
            }
            if (mtfWeeksRemDatum.type != TOML_INT64 || mtfWeeksRemDatum.u.int64 < 0) {
                log_error("MTF weeks remaining Must be an integer >= 0!\n");
                toml_free(tomlResult);
                return EOF;
            }
            if (mtfOperationalDatum.type != TOML_INT64 || mtfOperationalDatum.u.int64 < 0) {
                log_error("MTF operational Must be an integer >= 0!\n");
                toml_free(tomlResult);
                return EOF;
            }
            if (mtfWoundedDatum.type != TOML_INT64 || mtfWoundedDatum.u.int64 < 0) {
                log_error("MTF wounded Must be an integer >= 0!\n");
                toml_free(tomlResult);
                return EOF;
            }
            if (mtfDeadDatum.type != TOML_INT64 || mtfDeadDatum.u.int64 < 0) {
                log_error("MTF dead Must be an integer >= 0!\n");
                toml_free(tomlResult);
                return EOF;
            }

            progress.org.mtfs[i] = mtfIDDatum.u.int64;
            allMTFs[mtfIDDatum.u.int64].active = mtfActiveDatum.u.boolean;
            allMTFs[mtfIDDatum.u.int64].deployed = mtfDeployedDatum.u.boolean;
            allMTFs[mtfIDDatum.u.int64].deploymentWeeksRemaining = mtfWeeksRemDatum.u.int64;
            allMTFs[mtfIDDatum.u.int64].personnel.operational = mtfOperationalDatum.u.int64;
            allMTFs[mtfIDDatum.u.int64].personnel.wounded = mtfWoundedDatum.u.int64;
            allMTFs[mtfIDDatum.u.int64].personnel.dead = mtfDeadDatum.u.int64;
        }
    }

    toml_free(tomlResult);
    return 0;
}

void present_menu() {
    bool exit = false;
    while (!exit) {
        clear_screen();
        printf("[0] Set Player Name\n[1] New Game\n[2] Load Game\n[3] Exit Game\n[4] Save Game (DEBUG)\n");
        int selection = -1;
        int count = fscanf(stdin, "%d", &selection);
        fscanf(stdin, "%*[^\n]");
        if (count <= 0) {
            log_error("Could not read input!\n");
            return;
        }
        switch (selection) {
            case 0:
                printf("Enter name: ");
                int read = fscanf(stdin, "%63s", playerName);
                if (read <= 0) {
                    log_error("Could not read input!\n");
                } else {
                    printf("Your name is now set to %s\n", playerName);
                }
                break;
            case 1:
                progress.currentRoom = 1;
                play_game();
                break;
            case 2:
                printf("Loading...\n");
                if (load_game(saveFileName) != EOF) {
                    print_organisation_details(&progress.org);
                    play_game();
                }
                break;
            case 3:
                printf("Exiting...\n");
                exit = true;
                break;
            case 4:
                save_game(saveFileName);
                break;
            default:
                log_error("Invalid input!\n");
        }
    }
}
