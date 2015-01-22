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

#include <mumble/external.h>

/**
* @file mumble.h
* @author Mikkel Kroman
* @date 11 Dec 2014
* @brief Client-related functions for the mumble client context.
*/

#pragma once
#ifndef MUMBLE_H
#define MUMBLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
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
    /** Pointer to a path to the client private key. */
    const char* key_file;
    /** Pointer to a path to the client certificate. */
    const char* cert_file;
} mumble_settings_t;

/**
 * Create a new mumble client.
 * 
 * @param[in] settings a mumble settings structure.
 *
 * @see mumble_settings_t
 * @returns a pointer to an instantiated client. use `mumble_free` to destroy
 *   this client.
 */
MUMBLE_API struct mumble_t* mumble_new(mumble_settings_t settings);

/**
 * Close all connections and free all memory.
 *
 * @param[in] client a pointer to a client.
 */
MUMBLE_API void mumble_free(struct mumble_t* client);

/**
 * @brief Connect to a mumble server.
 *
 * @param client the mumble client.
 * @param server the server to connect to. can be created using
 *   `mumble_server_new`.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_connect(struct mumble_t* client, struct mumble_server_t* server);

/**
 * Send the client version message to the server.
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
