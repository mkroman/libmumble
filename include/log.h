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

#define ANSI_BOLD "\e[1m"
#define ANSI_RESET "\e[0m"

/**
 * 03:40:00	mumble_s...connected Connection established to uplink.io:64738.
 * 00:00:00	dddddddddddddddddddd Hello world.
 * 03:41:00	mumble_se...e_packet Unknown packet type 5.
 */

void mumble_log(const char* func, const char* format, ...);

#define MUMBLE_LOG(format, ...) \
	mumble_log(__FUNCTION__, format, ## __VA_ARGS__);

#endif
