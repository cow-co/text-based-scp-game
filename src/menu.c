#include "menu.h"
#include "gameplay.h"
#include "util.h"
#include <stdio.h>


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
