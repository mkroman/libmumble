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
#include "log.h"

static mumble_t g_mumble;

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

	mumble_init(&g_mumble, settings);
	mumble_connect(&g_mumble, host, 64738);
	mumble_run(&g_mumble);
	mumble_destroy(&g_mumble);

	return 0;
}
