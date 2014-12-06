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
 * @file connection.h
 * @author Mikkel Kroman
 * @date 5 Dec 2014
 * @brief Connection-related functions for the mumble context.
 */

#pragma once

#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The client-to-server connection struct.
 */
typedef struct mumble_connection_t
{
	const char* host;
	uint32_t port;
	int fd;
	struct sockaddr_storage addr;
	struct mumble_connection_t* next;
} mumble_connection_t;

#ifdef __cplusplus
}
#endif
