#include "menu.h"
#include "gameplay.h"
#include "util.h"
#include "tomlc17.h"
#include <stdio.h>
#include <string.h>


const char saveFileName[] = "data/save.toml";


void save_game() {
    FILE* file = fopen(saveFileName, "w");

    if (!file) {
        log_error("Could not open file for writing!\n");
        return;
    }

    fprintf(file, "narrative_flags = %ld\n", (long)(progress.narrativeFlags));
    fprintf(file, "player_name = \"%s\"\n", playerName);
    fprintf(file, "current_room = %zu\n", progress.currentRoom);
    
    fprintf(file, "[organisation]\n");
    fprintf(file, "reputation = %u\n", progress.org.reputation);
    fprintf(file, "budget = %u\n", progress.org.budget);
    fprintf(file, "scps = [");
    
    size_t countSCPs = sizeof(progress.org.scps) / sizeof(unsigned);
    bool isStart = true;
    for (size_t i = 0; i < countSCPs; ++i) {
        if(progress.org.scps[i]) {
            if (!isStart) {
                fprintf(file, ",");
            }
            fprintf(file, "%u", progress.org.scps[i]);
        } 
    }
    fprintf(file, "]\n");
    
    size_t countMTFs = sizeof(progress.org.mtfs) / sizeof(progress.org.mtfs[0]);
    for (size_t i = 0; i < countMTFs; ++i) {
        if (progress.org.mtfs[i]) {
            const unsigned id = progress.org.mtfs[i];
            fprintf(file, "[[mtf]]\n");
            fprintf(file, "id = %u\n", id);
            fprintf(file, "operational = %d\n", allMTFs[id].personnel.operational);
            fprintf(file, "wounded = %d\n", allMTFs[id].personnel.wounded);
            fprintf(file, "dead = %d\n", allMTFs[id].personnel.dead);
        }
    }
    fclose(file);
}


void load_game() {
    const toml_result_t tomlResult= toml_parse_file_ex(saveFileName);
    if (!tomlResult.ok) {
        log_error("Could not read saved-game file!\n");
        printf("%s\n", tomlResult.errmsg);
        toml_free(tomlResult);
        return;
    }
    
    const toml_datum_t nameDatum = toml_get(tomlResult.toptab, "player_name");
    const toml_datum_t roomDatum = toml_get(tomlResult.toptab, "current_room");
    const toml_datum_t narrativeDatum = toml_get(tomlResult.toptab, "narrative_flags");
    const toml_datum_t orgDatum = toml_get(tomlResult.toptab, "organisation");

    const size_t maxNameLen = sizeof(playerName);
    if (nameDatum.type != TOML_STRING || nameDatum.u.str.len >= maxNameLen) {
        log_error("Name field is not a string, or is too long!\n");
        toml_free(tomlResult);
        return;
    }
    if (roomDatum.type != TOML_INT64) {
        log_error("Room field is not an integer!\n");
        toml_free(tomlResult);
        return;
    }
    if (narrativeDatum.type != TOML_INT64) {
        log_error("Narrative flags field is not an integer!\n");
        toml_free(tomlResult);
        return;
    }
    if (orgDatum.type != TOML_TABLE) {
        log_error("Organisation field is not a table!\n");
        toml_free(tomlResult);
        return;
    }

    strncpy(playerName, nameDatum.u.str.ptr, nameDatum.u.str.len);
    progress.currentRoom = roomDatum.u.int64;
    progress.narrativeFlags = (_BitInt(64))narrativeDatum.u.int64;

    // TODO read organisation details

    toml_free(tomlResult);
}


void present_menu() {
    bool exit = false;
    while (!exit) {
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
                play_game();
                break;
            case 2:
                printf("Loading...\n");
                load_game();
                break;
            case 3:
                printf("Exiting...\n");
                exit = true;
                break;
            case 4:
                save_game();
                break;
            default:
                log_error("Invalid input!\n");
        }
    }
}
