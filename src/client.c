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

int main(int argc, char** argv)
{
    static const char* kDefaultHost = "chronicle.nodes.uplink.io";
    const char* host = kDefaultHost;

    if (argc > 1)
        host = argv[1];

    mumble_settings_t settings;

    settings.key_file = "private.key";
    settings.cert_file = "public.crt";

    LOG_INFO("libmumble v0.1");

    struct mumble_t* client = mumble_new(settings);
    struct mumble_server_t* server1 = mumble_server_new(host, 64738);
    struct mumble_server_t* server2 = mumble_server_new("127.0.0.1", 64738);

    if (mumble_connect(client, server1) != 0)
        LOG_ERROR("Something went wrong.");

    if (mumble_connect(client, server2) != 0)
        LOG_ERROR("Something went wrong.");

    mumble_run(client);
    mumble_free(client);

    return 0;
}
