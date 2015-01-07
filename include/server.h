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
#ifndef MUMBLE_SERVER_H
#define MUMBLE_SERVER_H

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

#include "protocol.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

/**
 * The default buffer size.
 */
static const size_t kMumbleBufferSize = 1024;

/**
 * The mumble message header length.
 */
static const size_t kMumbleHeaderSize = (sizeof(uint16_t) + sizeof(uint32_t));

/**
 * The client name to be sent in the version message.
 */
static const char* kMumbleClientName = "libmumble (github.com/mkroman/libmumble)";

struct mumble_t;

typedef struct simple_buffer
{
	char data[kMumbleBufferSize];
	size_t pos;
	size_t size;
} simple_buffer_t;

typedef struct mumble_server_t
{
	const char* host;
	uint32_t port;
	struct sockaddr_storage socket_address;
	socklen_t socket_addrlen;
	socket_t fd;
	SSL* ssl;
	ev_io watcher;
	struct simple_buffer read_buffer;
	struct simple_buffer write_buffer;
	struct mumble_t* ctx;
	struct mumble_server_t* next;
} mumble_server_t;

socket_t mumble_server_create_socket();

/**
 * Create a socket and connect to the remote host.
 *
 * @param[in] server a pointer to an initialized server struct.
 * @param[in] context a pointer to the initialized mumble context.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_server_connect(mumble_server_t* server, struct mumble_t* context);

/**
 * Initialize a server struct.
 *
 * @param[in] context a pointer to an initialized mumble context.
 * @param[in] server a pointer to allocated memory space.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_server_init(struct mumble_t* context, mumble_server_t* server);

/**
 * Initialize SSL on a server struct.
 *
 * @param[in] server a pointer to a mumble server struct.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_server_init_ssl(mumble_server_t* server);

int mumble_server_read_message(mumble_server_t* server, uint16_t type, 
							   uint32_t length);

void mumble_server_callback(EV_P_ ev_io *w, int revents);
void mumble_server_handshake(EV_P_ ev_io *w, int revents);

int mumble_server_send(mumble_server_t* server,
					   mumble_packet_type_t packet_type, void* message);
int mumble_server_send_version(mumble_server_t* server);
int mumble_server_send_authenticate(mumble_server_t* server, 
									const char* username, const char* password);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MUMBLE_SERVER_H */
