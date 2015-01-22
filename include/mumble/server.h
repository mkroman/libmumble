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
#include <string.h>

#ifdef _WIN32
# include <winsock2.h>
# include <ws2tcpip.h>
# include <windows.h>
# pragma comment(lib, "Ws2_32.lib")
#elif defined(__unix__) /* _WIN32 */
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
#endif /* defined(__unix__) */

#include <ev.h>
#include <openssl/ssl.h>

#include <mumble/mumble.h>
#include <mumble/external.h>

/**
 * @file server.h
 * @author Mikkel Kroman
 * @date 10 Dec 2014
 * @brief Connection-related functions for the mumble context.
 */

#pragma once
#ifndef MUMBLE_SERVER_H
#define MUMBLE_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct mumble_server_t
 * Opaque pointer object for working with and manipulating servers using the
 * public libmumble API.
 *
 * To create a new server instance, use `mumble_server_new`.
 *
 * Once this object has been created, you can pass it as an argument to
 * `mumble_connect`.
 */
struct mumble_server_t;

/**
 * Initialization macro for callbacks structure.
 */
#define MUMBLE_CALLBACK_INIT \
        { NULL, NULL }

/**
 * Generic callback function, taking a single opaque server pointer as argument.
 */
typedef int (*mumble_cb_server)(struct mumble_server_t*);

/**
 * Callback structure.
 *
 * To associate callbacks with a server, use the `mumble_server_set_callbacks`
 * function.
 *
 * @see mumble_server_set_callbacks
 */
struct mumble_callback_t
{
   /**
    * @brief Server connect callback.
    *
    * The `on_connect` function is called as soon as the SSL handshake is
    * complete and the connection has been established.
    *
    * @param server an opaque pointer type to a server structure.
    */
    mumble_cb_server on_connect;    

   /**
    * @brief Server disconnect callback.
    *
    * The `on_disconnect` function is called when the connection was abruply
    * closed.
    *
    * @param server an opaque pointer type to a server structure.
    */
    mumble_cb_server on_disconnect;
};

/**
 * Instantiates a new server, with a given host and port.
 *
 * @params[in] host the remote host.
 * @params[in] port the remote port.
 *
 * @returns an opaque pointer type pointing to a newly allocated server, or NULL
 * if failure when trying to allocate memory.
 */
MUMBLE_API struct mumble_server_t* mumble_server_new(const char* host,
                                                     uint32_t port);

/**
 * Close the connection and free all used memory.
 */
MUMBLE_API void mumble_server_free(struct mumble_server_t* server);

/**
 * Set the callback handlers for this server.
 */
MUMBLE_API void
mumble_server_set_callbacks(struct mumble_server_t* server,
                            const struct mumble_callback_t* callbacks);

/**
 * Get a pointer to a user with the specified session id.
 *
 * Note that the user id field is different from the session id field.
 *
 * If you want to retrieve a pointer to a user with a specific session id, use
 * `mumble_server_get_user_by_session_id`.
 *
 * @param[in] server  an opaque pointer type pointing to a server structure.
 * @param[in] user_id the users id.
 *
 * @returns a const pointer to a user if found, NULL otherwise.
 */
MUMBLE_API const struct mumble_user_t*
mumble_server_get_user_by_id(struct mumble_server_t* server, uint32_t user_id);

/**
 * Get a pointer to a user with the specified session id.
 *
 * @param[in] server     an opaque pointer type pointing to a server structure.
 * @param[in] session_id the session id.
 *
 * @returns a const pointer to a user if found, NULL otherwise.
 */
MUMBLE_API const struct mumble_user_t*
mumble_server_get_user_by_session_id(struct mumble_server_t* server,
                                     uint32_t session_id);

/**
 * Get a pointer to a user with the specified user name.
 *
 * @param[in] server an opaque pointer type pointing to a server structure.
 * @param[in] name   the users name.
 *
 * @returns a const pointer to a user if found, NULL otherwise.
 */
MUMBLE_API const struct mumble_user_t*
mumble_server_get_user_by_name(struct mumble_server_t* server,
                               const char* name);

/**
 * Get the remote servers host or IP-address.
 *
 * @param[in] server an opaque pointer type pointing to a server structure.
 *
 * @returns a pointer to a string with the server hostname or IP-address.
 */
MUMBLE_API const char*
mumble_server_get_host(const struct mumble_server_t* server);

/**
 * Get the remote servers port.
 *
 * @param[in] server an opaque pointer type pointing to a server structure.
 *
 * @returns the remote servers port number.
 */
MUMBLE_API uint32_t
mumble_server_get_port(const struct mumble_server_t* server);

#ifdef __cplusplus
}
#endif

#endif

