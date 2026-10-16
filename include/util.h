#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>

typedef enum {
    LOG_LVL_INFO = 0,
    LOG_LVL_ERROR
} LogLevel;

void log_error(const char* const message);

/**
 * @brief Logs the given message to output stream(s), along with current time.
 * @details Currently just to std streams (stderr for ERROR, FATAL; else stdout), but in future also to a log file.
 */
void log_message(const LogLevel level, const char* const message);

void clear_array(const size_t arrayByteSize, void* arrayBytes);

void clear_screen();

/**
 * @brief Reads a number from stdin, and validates against the maximum and minimum
 * @returns long -1 if failed to read or failed validation
 */
int read_numeric_input(size_t max, size_t min);
#endif
