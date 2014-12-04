#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mumble_connection
{
	char* host;
	short port;
} mumble_connection_t;

#ifdef __cplusplus
}
#endif
