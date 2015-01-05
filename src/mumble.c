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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mumble.h"
#include "server.h"

int
mumble_init(mumble_t* context, const char* certificate_file, const char* key_file)
{
#ifdef _WIN32
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		fprintf(stderr, "mumble_init: Failed to initialize winsock\n");

		return 1;
	}
#endif /* _WIN32 */

	context->servers = 0;
	context->num_servers = 0;

	// Initialize SSL.
	SSL_library_init();

	// Initialize the SSL context.
	context->ssl_ctx = SSL_CTX_new(SSLv23_client_method());

	if (!context->ssl_ctx)
	{
		fprintf(stderr, "SSL_CTX_new failed\n");

		return 1;
	}

	if (!SSL_CTX_use_certificate_chain_file(context->ssl_ctx,
											certificate_file))
	{
		fprintf(stderr, "SSL_CTX_use_certificate_chain_file failed (%s)\n", certificate_file);

		return 1;
	}

	if (!SSL_CTX_use_PrivateKey_file(context->ssl_ctx,
									 key_file, SSL_FILETYPE_PEM))
	{
		fprintf(stderr, "SSL_CTX_use_PrivateKey_file failed\n");

		return 1;
	}

	if (!SSL_CTX_check_private_key(context->ssl_ctx))
	{
		fprintf(stderr, "invalid cert/key pair\n");

		return 1;
	}
	
	context->key_file = key_file;
	context->certificate_file = certificate_file;

	// Initialize a new event loop.
	context->loop = ev_loop_new(0);

	return 0;
}

int mumble_destroy(mumble_t* context)
{
	// Free SSL resources.
	SSL_CTX_free(context->ssl_ctx);
	
	// Free event resources.
	if (context->loop)
		ev_loop_destroy(context->loop);

	// Close any file descriptors.
	for (struct mumble_server_t* srv = context->servers; srv->next != NULL;
		 srv = srv->next)
	{
		close(srv->fd);
	}

	return 0;
}

int mumble_connect(mumble_t* context, const char* host, uint32_t port)
{
	mumble_server_t* srv = (mumble_server_t*)malloc(sizeof(mumble_server_t));

	if (mumble_server_init(srv) != 0)
	{
		fprintf(stderr, "mumble_server_init failed\n");

		return 1;
	}

	srv->host = host;
	srv->port = port;

	if (context->servers)
		srv->next = context->servers;
	else
		srv->next = NULL;

	context->servers = srv;
	context->num_servers++;

	if (mumble_server_connect(srv, context) != 0)
		return 1;

	return 0;
}

int mumble_run(mumble_t* context)
{
	ev_loop(context->loop, 0);

	return 0;
}
