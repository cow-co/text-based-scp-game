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
 * @detail Currently just to std streams (stderr for ERROR, FATAL; else stdout), but in future also to a log file.
 */
void log_message(const LogLevel level, const char* const message);

void clear_array(const size_t arrayByteSize, void* arrayBytes);

#endif
