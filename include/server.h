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

#include "buffer.h"
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
 * The mumble message header length.
 */
static const size_t kMumbleHeaderSize = (sizeof(uint16_t) + sizeof(uint32_t));

/**
 * The client name to be sent in the version message.
 */
static const char* kMumbleClientName = "libmumble (github.com/mkroman/libmumble)";

struct mumble_t;

/**
 * The mumble server structure.
 */
typedef struct mumble_server_t
{
	/** The server host. */
	const char* host; 
	/** The server port. */
	uint32_t port;
	/** The socket file descriptor. */
	socket_t fd;
	/** The associated SSL object. */
	SSL* ssl;
	/** The I/O watcher for the socket file descriptor. */
	ev_io watcher;
	/** The periodic heartbeat timer. */
	ev_timer ping_watcher;
	/** The read buffer. */
	mumble_buffer_t rbuffer;
	/** The write buffer. */
	mumble_buffer_t wbuffer;
	/** A pointer to the client context. */
	struct mumble_t* ctx;
	/** A pointer to the next server in the linked list. */
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
 * Destroy a server struct.
 *
 * @param[in] server a pointer to the server struct to destroy.
 */
void mumble_server_destroy(mumble_server_t* server);

/**
 * Initialize SSL on a server struct.
 *
 * @param[in] server a pointer to a mumble server struct.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_server_init_ssl(mumble_server_t* server);

/**
 * Parse and handle a packet received from a server.
 *
 * @param[in] server a pointer to the server.
 * @param[in] type   the packet type.
 * @param[in] length the packet length.
 *
 * @returns one if successfully handled, zero otherwise.
 */
int mumble_server_handle_packet(mumble_server_t* server, uint16_t type,
								uint32_t length);

/**
 * Cut the data received from a server into packets and handle them as needed.
 *
 * @param[in] server a pointer to the server.
 *
 * @returns one if a packet was received, zero otherwise.,
 */
int mumble_server_read_packet(mumble_server_t* server);

/**
 * Called by the event loop when the server connection is readable or writable.
 *
 * @internal
 */
void mumble_server_callback(EV_P_ ev_io *w, int revents);

/**
 * Called by the event loop when establishing a secure connection in order to
 * perform the handshake.
 *
 * @internal
 */
void mumble_server_handshake(EV_P_ ev_io *w, int revents);

/**
 * Called when a connection to a server has been established.
 *
 * @param[in] server a pointer to the server.
 */
void mumble_server_connected(mumble_server_t* server);

/**
 * Called when a connection to a server has been lost.
 *
 * @param[in] server a pointer to the server.
 */
void mumble_server_disconnected(mumble_server_t* server);

/**
 * Send a packet to the server.
 *
 * @param[in] server a pointer to the server.
 * @param[in] packet_type the packet type.
 * @param[in] message a pointer to an initialized protobuf struct.
 *
 * @returns one if successful, zero otherwise.
 */
int mumble_server_send(mumble_server_t* server,
					   mumble_packet_type_t packet_type, void* message);

/**
 * Send a version packet to the server.
 *
 * @param[in] server a pointer to the server.
 *
 * @returns one if successful, zero otherwise.
 */
int mumble_server_send_version(mumble_server_t* server);

/**
 * Send a authentication packet to the server.
 *
 * @param[in] server a pointer to the server.
 * @param[in] username the client username.
 * @param[in] password the client password.
 *
 * @returns one if successful, zero otherwise.
 */
int mumble_server_send_authenticate(mumble_server_t* server, 
									const char* username, const char* password);

/**
 * Send a ping packet to the server.
 *
 * @param[in] server a pointer to the server.
 *
 * @returns one if successful, zero otherwise.
 */
int mumble_server_send_ping(mumble_server_t* server);

/**
 * Called periodically to send a ping packet to the server.
 *
 * @internal
 */
void mumble_server_ping(EV_P_ ev_timer* w, int revents);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MUMBLE_SERVER_H */
