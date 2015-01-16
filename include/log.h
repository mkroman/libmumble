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
#define ANSI_BOLD "\e[1m"
#define ANSI_RESET "\e[0m"
#define ANSI_BLACK "\e[30m"
#define ANSI_RED "\e[31m"
#define ANSI_GREEN "\e[32m"
#define ANSI_YELLOW "\e[33m"
#define ANSI_BLUE "\e[34m"
#define ANSI_MAGENTA "\e[35m"
#define ANSI_CYAN "\e[36m"
#define ANSI_WHITE "\e[37m"

/**
 * The log format.
 *
 * Arguments passed to printf where this is used are:
 * timestamp, file, level color, level string and message.
 */
#define LOG_FORMAT \
	ANSI_BOLD "%s" ANSI_RESET " %-15s %s" ANSI_BOLD "%-8s" ANSI_RESET "%s\n"

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
static const int kLogLevel = 5;

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

#define __FILENAME__ (strrchr(__FILE__, '/') ? \
					  strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_WRITE(level, format, ...) \
	do { log_write(level, __FUNCTION__, __FILENAME__, __LINE__, format, \
					## __VA_ARGS__); } while(0)

#define LOG_ERROR(format, ...) \
	LOG_WRITE(LOG_LEVEL_ERROR, format, ## __VA_ARGS__)

#define LOG_DEBUG(format, ...) \
	LOG_WRITE(LOG_LEVEL_DEBUG, format, ## __VA_ARGS__)

#define LOG_INFO(format, ...) \
	LOG_WRITE(LOG_LEVEL_INFO, format, ## __VA_ARGS__)

#define LOG_FATAL(format, ...) \
	LOG_WRITE(LOG_LEVEL_FATAL, format, ## __VA_ARGS__)

#define LOG_WARN(format, ...) \
	LOG_WRITE(LOG_LEVEL_WARN, format, ## __VA_ARGS__)

#define MUMBLE_ERR(format, ...) \
	LOG_ERROR(format, ## __VA_ARGS__)
	
#define MUMBLE_LOG(format, ...) \
	LOG_DEBUG(format, ## __VA_ARGS__)

#endif
