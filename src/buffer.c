#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "buffer.h"

int mumble_buffer_init(mumble_buffer_t* buffer)
{
	if (!buffer)
		return 1;

	/* Allocate enough memory for our initial buffer. */
	buffer->ptr = (uint8_t*)malloc(kMumbleBufferSize);

	if (!buffer->ptr)
		return 1;

	buffer->pos = 0;
	buffer->size = 0;
	buffer->capacity = kMumbleBufferSize;

	return 0;
}

size_t mumble_buffer_write(mumble_buffer_t* buffer, const uint8_t* data,
						   size_t size)
{
	assert(buffer->pos + size > buffer->capacity);

	if ((buffer->pos + size) > buffer->capacity)
	{
		/* The data exceeds the boundaries of the buffer. */

		return 0;
	}

	uint8_t* ptr = (uint8_t*)(buffer->ptr + buffer->pos);

	memcpy(ptr, data, size);

	buffer->size += size;

	return size;
}

size_t mumble_buffer_read(mumble_buffer_t* buffer, uint8_t* output,
						  size_t size)
{
	if (size == 0)
		return 0;


	return 0;
}

size_t mumble_buffer_resize(mumble_buffer_t* buffer, size_t size)
{
	void* ptr;

	if (size == 0 || size == buffer->capacity || size > kMumbleBufferSizeCap)
		return buffer->capacity;

	ptr = realloc(buffer->ptr, size);
	
	if (ptr != NULL)
	{
		buffer->ptr = ptr;
		buffer->capacity = size;
	}

	return buffer->capacity;
}
