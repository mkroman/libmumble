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

#include <mumble/mumble.h>

/**
* @file internal.h
* @author Mikkel Kroman
* @date 20 Jan 2015
* @brief Internal header for structured data.
*/

#ifdef __cplusplus
extern "C" {
#endif

#pragma once
#ifndef MUMBLE_INTERNAL_H
#define MUMBLE_INTERNAL_H

/**
 * The mumble client structure, also referenced to as the `client context`.
 *
 * To initialize a new instance of this struct, use mumble_init()
 */
struct mumble_t
{
    /**
     * Number of servers attached to this context.
     *
     * This is useful for getting the number of servers in constant time.
     */
    int num_servers;
    /** Pointer to an SSL context that will be inherited by new servers. */
    SSL_CTX* ssl_ctx;
    /** Pointer to the event loop this context operates on. */
    struct ev_loop* loop;
    /** Client settings for this context. */
    mumble_settings_t settings;
    /** Internal buffer that is used by the library. */
    char buffer[512];
    /** Linked list of servers attached to this client. */
    struct mumble_server_t* servers;
};

#endif

#ifdef __cplusplus
}
#endif
