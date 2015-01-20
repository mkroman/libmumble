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

#include <openssl/ssl.h>
#include <ev.h>

#include <mumble/mumble.h>

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

/*
 * Forward declarations.
 */
struct mumble_server_t;

/**
 * Instantiates a new server. Use this object with `mumble_connect` in order to
 * connect to a remote server.
 */
struct mumble_server_t* mumble_server_new(const char* host, uint32_t port);

/**
 * Close the connection and free all used memory.
 */
void mumble_server_free(struct mumble_server_t* server);

/**
 * Get the servers host or IP address.
 */
MUMBLE_API const char*
mumble_server_get_host(const struct mumble_server_t* server);

/**
 * Get the servers port.
 */
MUMBLE_API uint32_t
mumble_server_get_port(const struct mumble_server_t* server);

#ifdef __cplusplus
}
#endif

#endif
