#include "util.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

// TODO Make this a format message
void log_error(const char* const message) {
    log_message(LOG_LVL_ERROR, message);
}

// TODO Make this a format message
void log_message(const LogLevel level, const char* const message) {
    // Later, can do more here like also logging to a file
    time_t current = time(NULL);
    struct tm* local = localtime(&current);
    char timeText[64] = {0};
    const size_t written = strftime(timeText, sizeof(timeText), "%c", local);
    if (written) {
        if (level == LOG_LVL_ERROR) {
            fprintf(stderr, "[%s] [ERROR] %s", timeText, message);
        } else {
            fprintf(stdout, "[%s] [INFO] %s", timeText, message);
        }
    }
}

void clear_array(const size_t arrayByteSize, void* arrayBytes) {
    if (arrayByteSize > 0) {
        memset(arrayBytes, 0, arrayByteSize);
    }
}

void clear_screen() {
    printf("\e[H\e[2J\e[3J");   // The sequence used by `clear` in the terminal
}

int read_numeric_input(unsigned max, unsigned min) {
    // NOTE: if min/max > INT_MAX then this can overflow, but in practice we won't have that (all options should be < INT_MAX)
    int selection = -1;
    const int count = fscanf(stdin, "%d", &selection);
    if (count <= 0) {
        log_error("Could not read input!\n");
        return -1;
    }
    fscanf(stdin, "%*[^\n]");
    if (selection > max || selection < min) {
        log_error("Invalid input!\n");
        return -1;
    }
    return selection;
}