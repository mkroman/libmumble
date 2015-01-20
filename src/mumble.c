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

#include <mumble/mumble.h>
#include <mumble/server.h>
#include "iserver.h"
#include "internal.h"
#include "log.h"

const mumble_version_t kMumbleClientVersion = {1, 2, 8};

struct mumble_t* mumble_new(mumble_settings_t settings)
{
    struct mumble_t* client = (struct mumble_t*)malloc(sizeof(struct mumble_t));

    if (!client)
        return NULL;

    client->settings = settings;

    if (mumble_init(client) != 0)
        return NULL;

    return client;
}

int mumble_init(struct mumble_t* client)
{
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        LOG_FATAL("Failed to initialize winsock");

        return 1;
    }
#endif /* _WIN32 */

    client->servers = NULL;
    client->num_servers = 0;

    if (mumble_ssl_init(client) != 0)
        return 1;

    /* Initialize a new event loop. */
    client->loop = ev_loop_new(0);

    return 0;
}

int mumble_ssl_init(struct mumble_t* client)
{
    /* Initialize the SSL library. */
    SSL_library_init();
    SSL_load_error_strings();

    /* Initialize the SSL context. */
    client->ssl_ctx = SSL_CTX_new(SSLv23_client_method());

    if (!client->ssl_ctx)
    {
        LOG_ERROR("SSL_CTX_new failed");

        return 1;
    }

    if (!SSL_CTX_use_certificate_chain_file(client->ssl_ctx,
                                            client->settings.cert_file))
    {
        LOG_ERROR("SSL_CTX_use_certificate_chain_file failed (%s)",
                  client->settings.cert_file);

        return 1;
    }

    if (!SSL_CTX_use_PrivateKey_file(client->ssl_ctx, client->settings.key_file,
                                     SSL_FILETYPE_PEM))
    {
        LOG_ERROR("SSL_CTX_use_PrivateKey_file failed");

        return 1;
    }

    if (!SSL_CTX_check_private_key(client->ssl_ctx))
    {
        LOG_ERROR("Invalid cert/key pair");

        return 1;
    }

    return 0;
}

void mumble_free(struct mumble_t* client)
{
    struct mumble_server_t* ptr, *next;
    for (ptr = client->servers; ptr != NULL; ptr = next)
    {
        next = ptr->next;
        mumble_server_free(ptr);
    }

    /* Free SSL resources. */
    SSL_CTX_free(client->ssl_ctx);

    /* Close any open connections and stop the event loop. */
    if (client->loop)
        ev_loop_destroy(client->loop);

    free(client);
}

int mumble_connect(struct mumble_t* client, struct mumble_server_t* server)
{
    if (!client || !server)
        return 1;

    if (client->servers)
        server->next = client->servers;
    else
        server->next = NULL;

    client->num_servers++;
    client->servers = server;
    server->client = client;

    if (mumble_server_connect(server) != 0)
        return 1;

    return 0;
}

int mumble_run(struct mumble_t* client)
{
    ev_loop(client->loop, 0);

    return 0;
}
