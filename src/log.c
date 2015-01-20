#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "log.h"

/**
 * Strings representing each debug level.
 */
static const char* g_log_level_strings[] = {
    "Fatal",   /* LOG_LEVEL_FATAL */
    "Error",   /* LOG_LEVEL_ERROR */
    "Warning", /* LOG_LEVEL_WARN */
    "Debug",   /* LOG_LEVEL_DEBUG */
    "Info",    /* LOG_LEVEL_INFO */
    0
};

/**
 * ANSI colors for the different log levels.
 */
static const char* g_log_level_colors[] = {
    ANSI_RED,    /* LOG_LEVEL_FATAL */
    ANSI_RED,    /* LOG_LEVEL_ERROR */
    ANSI_YELLOW, /* LOG_LEVEL_WARN */
    ANSI_WHITE,  /* LOG_LEVEL_DEBUG */
    ANSI_BLUE,   /* LOG_LEVEL_INFO */
    0
};

/**
 * The default log-level.
 */
static const int kLogLevel = LOG_LEVEL;

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static size_t log_write_timestamp(char* buffer, size_t size)
{
    size_t result;
    time_t current_time = time(NULL);
    struct tm local_time;

    if (localtime_r(&current_time, &local_time) == NULL)
        return 0;

    result = strftime(buffer, size, "%X", &local_time);

    return result;
}

void log_write(log_level_t level, const char* func, const char* file,
               unsigned long line, const char* format, ...)
{
    (void)func;
    static char time_buffer[12];
    static char message_buffer[1024];
    static char file_buffer[21];
    va_list ap;

    if (level > kLogLevel)
        return;

    va_start(ap, format);
    pthread_mutex_lock(&log_mutex);

    /* Write the file name and line number into `file_buffer.` */
    snprintf(file_buffer, sizeof file_buffer, "%s:%lu", file, line);

    /* Format and write the log message into `message_buffer.` */
    vsnprintf(message_buffer, sizeof message_buffer, format, ap);

    /* Write a timestamp into `time_buffer.` */
    log_write_timestamp(time_buffer, sizeof time_buffer);

    /* Now put it all together and print it. */
    printf(LOG_FORMAT, time_buffer, file_buffer, g_log_level_colors[level - 1],
           g_log_level_strings[level - 1], message_buffer);

    pthread_mutex_unlock(&log_mutex);
    va_end(ap);
}

