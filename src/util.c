#include "util.h"
#include <stdio.h>
#include <time.h>

void log_error(const char* const message) {
    log_message(LOG_LVL_ERROR, message);
}

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
