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
 * @file iserver.h
 * @author Mikkel Kroman
 * @date 20 Jan 2015
 * @brief Internal server structures and functions.
 */

#pragma once
#ifndef MUMBLE_INTERNAL_SERVER_H
#define MUMBLE_INTERNAL_SERVER_H

#include <mumble/server.h>

#include "buffer.h"
#include "protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

/**
 * @private
 * The mumble server structure.
 */
struct mumble_server_t
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
    ev_timer ping_timer;
    /** The read buffer. */
    mumble_buffer_t rbuffer;
    /** The write buffer. */
    mumble_buffer_t wbuffer;
    /** A pointer to the client context this server belongs to. */
    struct mumble_t* client;
    /** The connection session id. */
    int session;
    /** The maximum bandwidth we're allowed to use. */
    int max_bandwidth;
    /** The servers welcome text. */
    char* welcome_text;
    /** The servers permission flags. */
    uint64_t permissions;
    /** A pointer to a list of callback handlers. */
    struct mumble_callback_t callbacks;
    /** A pointer to a linked list with channels. */
    struct mumble_channel_t* channels;
    /** A pointer to a linked list with users. */
    struct mumble_user_t* users;
    /** A pointer to the next server in the linked list. */
    struct mumble_server_t* next;
};

/**
 * @private
 */
socket_t mumble_server_create_socket();

/**
 * @private
 * Create a socket and connect to the remote host.
 *
 * @param[in] server a pointer to an initialized server struct.
 * @param[in] context a pointer to the initialized mumble context.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_server_connect(struct mumble_server_t* server);

/**
 * @private
 * Initialize a server struct.
 *
 * @param[in] server a pointer to allocated memory space.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_server_init(struct mumble_server_t* server);


/**
 * @private
 * Initialize SSL on a server struct.
 *
 * @param[in] server a pointer to a mumble server struct.
 *
 * @returns zero on success, non-zero otherwise.
 */
int mumble_server_ssl_init(struct mumble_server_t* server);

/**
 * @private
 * Forcefully close the connection.
 *
 * @param[in] server a pointer to the server.
 */
void mumble_server_close(struct mumble_server_t* server);

/**
 * @private
 * Parse and handle a packet received from a server.
 *
 * @param[in] server a pointer to the server.
 * @param[in] type   the packet type.
 * @param[in] length the packet length.
 *
 * @returns one if successfully handled, zero otherwise.
 */
int mumble_server_handle_packet(struct mumble_server_t* server, uint16_t type,
                                uint32_t length);

/**
 * @private
 * Cut the data received from a server into packets and handle them as needed.
 *
 * @param[in] server a pointer to the server.
 *
 * @returns one if a packet was received, zero otherwise.,
 */
int mumble_server_read_packet(struct mumble_server_t* server);

/**
 * @private
 * Called by the event loop when the server connection is readable or writable.
 */
void mumble_server_callback(EV_P_ ev_io* w, int revents);

/**
 * @private
 * Called by the event loop when establishing a secure connection in order to
 * perform the handshake.
 */
void mumble_server_handshake(EV_P_ ev_io* w, int revents);

/**
 * @private
 * Called when a connection to a server has been established.
 *
 * @param[in] server a pointer to the server.
 */
void mumble_server_connected(struct mumble_server_t* server);

/**
 * @private
 * Called when a connection to a server has been lost.
 *
 * @param[in] server a pointer to the server.
 */
void mumble_server_disconnected(struct mumble_server_t* server);

/**
 * @private
 * Send a packet to the server.
 *
 * @param[in] server a pointer to the server.
 * @param[in] packet_type the packet type.
 * @param[in] message a pointer to an initialized protobuf struct.
 *
 * @returns one if successful, zero otherwise.
 */
int mumble_server_send(struct mumble_server_t* server,
                       mumble_packet_type_t packet_type, void* message);

/**
 * @private
 * Send a version packet to the server.
 *
 * @param[in] server a pointer to the server.
 *
 * @returns one if successful, zero otherwise.
 */
int mumble_server_send_version(struct mumble_server_t* server);

/**
 * @private
 * Send a authentication packet to the server.
 *
 * @param[in] server a pointer to the server.
 * @param[in] username the client username.
 * @param[in] password the client password.
 *
 * @returns one if successful, zero otherwise.
 */
int mumble_server_send_authenticate(struct mumble_server_t* server,
                                    const char* username, const char* password);

/**
 * @private
 * Send a ping packet to the server.
 *
 * @param[in] server a pointer to the server.
 *
 * @returns one if successful, zero otherwise.
 */
int mumble_server_send_ping(struct mumble_server_t* server);

/**
 * @private
 * Called periodically to send a ping packet to the server.
 */
void mumble_server_ping(EV_P_ ev_timer* w, int revents);

#ifdef __cplusplus
}
#endif

#endif
