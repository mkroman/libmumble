/*
 * libmumble
 * Copyright (c) 2014 Mikkel Kroman, All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file log.h
 * @author Mikkel Kroman
 * @date 15 Jan 2015
 * @brief Logging facility for debugging purposes.
 */

#pragma once
#ifndef MUMBLE_LOG_H
#define MUMBLE_LOG_H

/*
 * Some simple macros to help with ANSI escape sequences.
 */
#define ANSI_BOLD "\x1B[1m"
#define ANSI_RESET "\x1B[0m"
#define ANSI_BLACK "\x1B[30m"
#define ANSI_RED "\x1B[31m"
#define ANSI_GREEN "\x1B[32m"
#define ANSI_YELLOW "\x1B[33m"
#define ANSI_BLUE "\x1B[34m"
#define ANSI_MAGENTA "\x1B[35m"
#define ANSI_CYAN "\x1B[36m"
#define ANSI_WHITE "\x1B[37m"

/**
 * The log format.
 *
 * Arguments passed to printf where this is used are:
 * timestamp, file, level color, level string and message.
 */
#define LOG_FORMAT                                                             \
    ANSI_BOLD "%s" ANSI_RESET " %-15s %s" ANSI_BOLD "%-8s" ANSI_RESET "%s\n"

#ifndef LOG_LEVEL
# define LOG_LEVEL 4
#endif

/**
 * The different levels of logging.
 */
typedef enum log_level_t
{
    LOG_LEVEL_FATAL = 1,
    LOG_LEVEL_ERROR = 2,
    LOG_LEVEL_WARN  = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_INFO  = 5
} log_level_t;

/**
 * Write a log message.
 *
 * The log format is:
 * 00:00:00 file.c:10       Debug    Message
 *
 * Where 00:00:00 is the current time, file.c is the source file, 10 is the
 * source line, Debug is the log level and Message is the message provided in
 * `format`.
 *
 * @param[in] level  the log level.
 * @param[in] func   the current function name.
 * @param[in] file   the current source file.
 * @param[in] line   the current source line.
 * @param[in] format the message format.
 * @param[in] ...    arguments to be formatted into the message.
 */
void log_write(log_level_t level, const char* func, const char* file,
               unsigned long line, const char* format, ...);

#define __FILENAME__                                                           \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_WRITE(level, ...)                                                  \
    do                                                                         \
    {                                                                          \
        log_write(level, __FUNCTION__, __FILENAME__, __LINE__, __VA_ARGS__);   \
    } while (0)

#if LOG_LEVEL >= 1
#define LOG_FATAL(...) LOG_WRITE(LOG_LEVEL_FATAL, __VA_ARGS__)
#else
#define LOG_FATAL(...)
#endif

#if LOG_LEVEL >= 2
#define LOG_ERROR(...) LOG_WRITE(LOG_LEVEL_ERROR, __VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif

#if LOG_LEVEL >= 3
#define LOG_WARN(...) LOG_WRITE(LOG_LEVEL_WARN, __VA_ARGS__)
#else
#define LOG_WARN(...)
#endif

#if LOG_LEVEL >= 4
#define LOG_DEBUG(...) LOG_WRITE(LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#if LOG_LEVEL >= 5
#define LOG_INFO(...) LOG_WRITE(LOG_LEVEL_INFO, __VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#endif
