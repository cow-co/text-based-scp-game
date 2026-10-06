#include "menu.h"
#include "gameplay.h"
#include "util.h"
#include <stdio.h>


const char saveFileName[] = "data/save.toml";


void save_game() {
    FILE* file = fopen(saveFileName, "w");

    if (!file) {
        log_error("Could not open file for writing!\n");
        return;
    }

    fprintf(file, "narrative_flags = %ld\n", (long)(progress.narrativeFlags));
    fprintf(file, "player_name = %s\n", playerName);
    fprintf(file, "current_room = %zu\n", progress.currentRoom);
    fprintf(file, "[organisation]\n");
    fprintf(file, "reputation = %u\n", progress.org.reputation);
    fprintf(file, "budget = %u\n", progress.org.budget);
    // TODO Calculate countSCPs
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
    // TODO Calculate countMTFs
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
    // TODO Parse file
    // TODO Set player name, narrative flags, current room
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
                printf("Placeholder...\n");
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
