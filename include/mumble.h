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
#ifndef MUMBLE_H
#define MUMBLE_H

#include <stdint.h>
#include <openssl/ssl.h>

#define MUMBLE_API

#include "server.h"

#define LOG(...)                      \
	do {                              \
		fprintf(stdout, __VA_ARGS__); \
	} while (0)

#define ERR(...)                      \
	do {                              \
		fprintf(stderr, __VA_ARGS__); \
	} while (0)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * The mumble client structure, also referenced as the `context`.
 */
typedef struct mumble_t
{
	int num_servers;
	SSL_CTX* ssl_ctx;
	struct ev_loop* loop;
	const char* certificate_file;
	const char* key_file;
	mumble_server_t* servers;
} mumble_t;

/**
 * Initialize a new mumble client context.
 *
 * @param context a pointer to allocated memory large enough to hold mumble_t.
 * @param cert_file a pointer to a string containing the file path to a client
 *   certificate.
 * @param key_file a pointer to a string containing the file path to a private
 *   key.
 *
 * @returns zero on success, non-zero otherwise.
 */
MUMBLE_API int
mumble_init(mumble_t* context, const char* cert_file, const char* key_file);

/**
 * Initialize the SSL context of a mumble context.
 *
 * @param[in] context a pointer to the mumble context.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_init_ssl(mumble_t* context);

/**
 * Destroy the mumble context, freeing all associated resources.
 *
 * @param context a pointer to the initialized mumble client.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_destroy(mumble_t* context);

/**
 * @brief Connect to a mumble server.
 *
 * @param context the initialized mumble client.
 * @param host    the hostname or address of the remote host.
 * @param port    the port of the remote host.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_connect(mumble_t* context, const char* host, uint32_t port);

/**
 * Send the client version message to the server.
 *
 */
int mumble_send_version(mumble_t* context, mumble_server_t* server);

/**
 * Run the main event loop.
 *
 * @param context the initialized mumble client.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_run(mumble_t* context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MUMBLE_H */
