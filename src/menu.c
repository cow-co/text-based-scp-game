#include "menu.h"
#include "gameplay.h"
#include "util.h"
#include "narrative.h"
#include "organisation.h"
#include <stdio.h>


const char saveFileName[] = "data/save.toml";


void save_game() {
    FILE* file = fopen(saveFileName, "w");
    fprintf(file, "narrative_flags = %ld\n", (long)flags);
    fprintf(file, "player_name = %s\n", playerName);
    fprintf(file, "current_room = %d\n", 1);
}


void load_game() {
    // TODO Parse file
    // TODO Set player name, narrative flags, current room
}


void present_menu() {
    bool exit = false;
    while (!exit) {
        printf("[0] Set Player Name\n[1] New Game\n[2] Load Game\n[3] Exit Game\n");
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
            default:
                log_error("Invalid input!\n");
        }
    }
}
