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

#include <stdint.h>
#include <openssl/ssl.h>

/**
* @file mumble.h
* @author Mikkel Kroman
* @date 11 Dec 2014
* @brief Client-related functions for the mumble client context.
*/

#ifdef __cplusplus
extern "C" {
#endif

#pragma once
#ifndef MUMBLE_H
#define MUMBLE_H

#ifdef __unix__
# define MUMBLE_API extern
#endif

/**
 * Forward declarations.
 */
struct mumble_t;
struct mumble_server_t;

/**
 * Mumble version struct.
 */
typedef struct mumble_version_t
{
    int major; /**< The major version number. */
    int minor; /**< The minor version number. */
    int patch; /**< The patch version number. */
} mumble_version_t;

/**
 * The mumble client version.
 */
MUMBLE_API const mumble_version_t kMumbleClientVersion;

/**
 * The mumble settings structure.
 *
 * This defines what parameters the client should use.
 */
typedef struct mumble_settings_t
{
    const char* key_file;  /**< pointer to a path to the client private key. */
    const char* cert_file; /**< pointer to a path to the client certificate. */
} mumble_settings_t;

/**
 * Create a new mumble client.
 */
MUMBLE_API struct mumble_t* mumble_new(mumble_settings_t settings);

/**
 * Close all connections, and free any memory used.
 */
MUMBLE_API void mumble_free(struct mumble_t* client);


/**
 * Initialize a new mumble client context.
 *
 * @param client   a pointer to allocated memory large enough to hold mumble_t.
 * @param settings a settings struct that defines how the client behaves.
 *
 * @returns zero on success, non-zero otherwise.
 */
MUMBLE_API int mumble_init(struct mumble_t* client);

/**
 * Destroy the mumble context, freeing all associated resources.
 *
 * @param context a pointer to the initialized mumble client.
 *
 * @returns zero on success, non-zero otherwise.
 */
// int mumble_destroy(mumble_t* context);

/**
 * @brief Connect to a mumble server.
 *
 * @param context the initialized mumble client.
 * @param host    the hostname or address of the remote host.
 * @param port    the port of the remote host.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_connect(struct mumble_t* client, struct mumble_server_t* server);

/**
 * Send the client version message to the server.
 *
 */
int mumble_send_version(struct mumble_t* context,
                        struct mumble_server_t* server);

/**
 * Run the main event loop.
 *
 * @param context the initialized mumble client.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_run(struct mumble_t* context);

#ifdef __cplusplus
}
#endif

#endif /* MUMBLE_H */
