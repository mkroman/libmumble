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
#include <mumble/server.h>
#include "log.h"

int server_on_connect(struct mumble_server_t* server)
{
    printf("Connected to %s!\n", mumble_server_get_host(server));

    return 0;
}

int server_on_disconnect(struct mumble_server_t* server)
{
    printf("Disconnected from %s!\n", mumble_server_get_host(server));

    return 0;
}

struct mumble_server_t* create_server(const char* host, uint32_t port)
{
    struct mumble_server_t* server = mumble_server_new(host, port);
    struct mumble_callback_t callbacks = MUMBLE_CALLBACK_INIT;

    callbacks.on_connect = server_on_connect;
    callbacks.on_disconnect = server_on_disconnect;

    if (server)
        mumble_server_set_callbacks(server, &callbacks);

    return server;
}

int main(int argc, char** argv)
{
    static const char* kDefaultHost = "chronicle.nodes.uplink.io";
    const char* host = kDefaultHost;

    if (argc > 1)
        host = argv[1];

    mumble_settings_t settings = {
        .key_file = "private.key",
        .cert_file = "public.crt"
    };

    LOG_INFO("libmumble v0.1");

    struct mumble_t* client = mumble_new(settings);
    struct mumble_server_t* server1 = create_server(host, 64738);
    struct mumble_server_t* server2 = create_server("127.0.0.1", 64738);

    LOG_INFO("Connecting to %s", mumble_server_get_host(server1));

    if (mumble_connect(client, server1) != 0)
        LOG_ERROR("Something went wrong.");

    if (mumble_connect(client, server2) != 0)
        LOG_ERROR("Something went wrong.");

    mumble_run(client);
    mumble_free(client);

    return 0;
}
