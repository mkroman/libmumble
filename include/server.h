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
* @file server.h
* @author Mikkel Kroman
* @date 10 Dec 2014
* @brief Connection-related functions for the mumble context.
*/

#pragma once

#include <stdint.h>
#include <string.h>

#include <openssl/ssl.h>
#include <ev.h>

#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  include <windows.h>
#  
#  pragma comment(lib, "Ws2_32.lib")
#elif defined(__unix__) /* _WIN32 */
#  include <arpa/inet.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#endif /* defined(__unix__) */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

static const int kBufferSize = 1024;

struct mumble_t;

typedef struct mumble_server_t
{
	const char* host;
	uint32_t port;
	struct sockaddr_storage socket_address;
	socklen_t socket_addrlen;
	socket_t fd;
	SSL* ssl;
	ev_io watcher;
	char buffer[kBufferSize];
	size_t buffer_pos;
	struct mumble_server_t* next;
} mumble_server_t;

socket_t
mumble_server_create_socket();

/**
 * Create a socket and connect to the remote host.
 *
 * @param[in] server a pointer to an initialized server struct.
 * @param[in] context a pointer to the initialized mumble context.
 *
 * @returns zero on success, non-zero otherwise.
 */
int
mumble_server_connect(mumble_server_t* server, struct mumble_t* context);

/**
 * Initialize a server struct.
 *
 * @param[in] server a pointer to allocated memory space.
 *
 * @returns zero on success, non-zero otherwise.
 */
int
mumble_server_init(mumble_server_t* server);

void mumble_server_callback(EV_P_ ev_io *w, int revents);
void mumble_server_handshake(EV_P_ ev_io *w, int revents);

#ifdef __cplusplus
}
#endif /* __cplusplus */
