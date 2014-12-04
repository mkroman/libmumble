#pragma once

#include "connection.h"

typedef struct mumble
{
	int num_connections;
	mumble_connection_t connection;
} mumble_t;

/// Initialize a new mumble context.
/// 
/// \returns non-zero on failure, zero on success.
int mumble_init(mumble_t* context);

/// Connect to a remote host.
int mumble_connect(const mumble_t* context, const char* host, short port);
