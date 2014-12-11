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
* @file mumble.h
* @author Mikkel Kroman
* @date 11 Dec 2014
* @brief Client-related functions for the mumble client context.
*/

#pragma once

#include <stdint.h>

#include "server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The mumble client structure, also referenced as the `context`.
 */
typedef struct
{
	int num_servers;
	mumble_server_t* servers;
} mumble_t;

/**
 * Initialize a new mumble client context.
 *
 * @param context a pointer to allocated memory large enough to hold mumble_t.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_init( mumble_t* context );

/**
 * @brief Connect to a mumble server.
 *
 * @param context the initialized mumble client.
 * @param host    the hostname or address of the remote host.
 * @param port    the port of the remote host.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_connect( mumble_t* context, const char* host, uint32_t port );

#ifdef __cplusplus
}
#endif