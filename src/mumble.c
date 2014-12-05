#include <stdlib.h>

#include "mumble.h"
#include "connection.h"

int mumble_init(mumble_t* context)
{
	context->connections = 0;
	context->num_connections = 0;
	
	return 0;
}

int mumble_connect(mumble_t* context, const char* host, uint32_t port)
{
	mumble_connection_t* conn = (mumble_connection_t*)malloc(
									sizeof(mumble_connection_t));

	conn->host = host;
	conn->port = port;
	conn->next = NULL;

	if (context->connections)
		conn->next = context->connections;

	context->connections = conn;
	context->num_connections++;

	return 0;
}
