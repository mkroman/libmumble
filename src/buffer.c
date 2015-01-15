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
	size_t buffer_capacity = buffer->capacity;

	if ((buffer->pos + size) > buffer->capacity)
	{
		/* The data exceeds the boundaries of the buffer, try to resize. */
		if (mumble_buffer_resize(buffer, buffer->capacity + size) == buffer_capacity)
			return 0;
	}

	uint8_t* ptr = (uint8_t*)(buffer->ptr + buffer->pos);

	memcpy(ptr, data, size);

	buffer->size += size;
	buffer->pos += size;

	return size;
}

size_t mumble_buffer_read(mumble_buffer_t* buffer, uint8_t* output,
						  size_t size)
{
	if (size == 0)
		return 0;

	if (size > buffer->capacity)
		size = buffer->capacity;

	if (output != NULL)
		memcpy(output, buffer->ptr, size);

	buffer->size -= size;
	buffer->pos -= size;
	memmove(buffer->ptr, (buffer->ptr + size), buffer->size);

	return size;
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
