#pragma once

#include <stdint.h>

#include "connection.h"

typedef struct mumble
{
	int num_connections;
	mumble_connection_t* connections;
} mumble_t;

/// Initialize a new mumble context.
/// 
/// \returns non-zero on failure, zero on success.
int mumble_init(mumble_t* context);

/// Connect to a remote host.
int mumble_connect(mumble_t* context, const char* host, uint32_t port);
