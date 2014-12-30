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

#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  include <windows.h>
#  
#  pragma comment(lib, "Ws2_32.lib")
#elif defined(__unix__)
#  include <arpa/inet.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

typedef struct mumble_server_t
{
	const char* host;
	uint32_t port;
	struct sockaddr_storage socket_address;
	socket_t fd;
	struct mumble_server_t* next;
} mumble_server_t;

/**
 * Get the address of the remote host.
 *
 * @param[in] server a pointer to an initialized server struct.
 * @param[out] addr allocated memory for the socket address.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_server_resolve_addr(mumble_server_t* server,
							   struct sockaddr_storage* addr);

/**
 * Create a socket and connect to the remote host.
 *
 * @param[in] server a pointer to an initialized server struct.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_server_connect(mumble_server_t* server);

#ifdef __cplusplus
}
#endif
