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

#pragma once

#include <stdint.h>

#include "server.h"

typedef struct mumble
{
	int num_servers;
	mumble_server_t* servers;
} mumble_t;

/// Initialize a new mumble context.
/// 
/// \returns non-zero on failure, zero on success.
int mumble_init( mumble_t* context );

/// Connect to a remote host.
int mumble_connect( mumble_t* context, const char* host, uint32_t port );
