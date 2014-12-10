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

#include <stdlib.h>

#include "mumble.h"
#include "server.h"

int mumble_init( mumble_t* context )
{
	context->servers = 0;
	context->num_servers = 0;

	return 0;
}

int mumble_connect( mumble_t* context, const char* host, uint32_t port )
{
	mumble_server_t* srv = ( mumble_server_t* )malloc( sizeof( mumble_server_t ) );

	srv->host = host;
	srv->port = port;

	if ( context->servers )
		srv->next = context->servers;
	else
		srv->next = NULL;

	context->servers = srv;
	context->num_servers++;

	return 0;
}
