#pragma once

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mumble_connection
{
	const char* host;
	uint32_t port;
	mumble_connection* next;
} mumble_connection_t;

#ifdef __cplusplus
}
#endif
