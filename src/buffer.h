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

/**
 * @file buffer.h
 * @author Mikkel Kroman
 * @date 08 Jun 2015
 * @brief Provide a buffer stored in memory with fast read and write access.
 */

#include <stddef.h>
#include <stdint.h>

#pragma once
#ifndef MUMBLE_BUFFER_H
#define MUMBLE_BUFFER_H

/**
 * The default buffer size for new buffers.
 */
static const size_t kMumbleBufferSize = 1024 * 8;

/**
 * The maximum size a buffer is allowed to resize to.
 */
static const size_t kMumbleBufferSizeCap = 1024 * 1024 * 8;

/**
 * The mumble buffer structure.
 *
 * This is used for keeping track of the buffers size, where the memory is
 * allocated, and so on.
 */
typedef struct mumble_buffer_t
{
    /** Pointer to allocated memory. */
    uint8_t* ptr;
    /** The capacity of the allocated memory, in number of bytes. */
    size_t capacity;
    /** The number of bytes that is currently used for storing data. */
    size_t size;
    /** The current write position. */
    size_t pos;
} mumble_buffer_t;

/**
 * Initialize a buffer.
 *
 * @param[in] buffer a pointer to memory space to initialize.
 *
 * @returns zero if success, non-zero otherwise.
 */
int mumble_buffer_init(mumble_buffer_t* buffer);

/**
 * Write data to the buffer.
 *
 * @param[in] buffer the buffer to write to.
 * @param[in] data   the data to write to the buffer.
 * @param[in] size   the size of the data parameter in bytes.
 *
 * @returns the number of bytes written.
 */
size_t mumble_buffer_write(mumble_buffer_t* buffer, const uint8_t* data,
                           size_t size);

/**
 * @brief Read data from the buffer into the `output` buffer.
 *
 * If output is `NULL`, then the number of bytes specified in `size` is simply
 *   discarded from the buffer.
 *
 * @param[in] buffer the buffer to read from.
 * @param[in] output a buffer for storing the data that is read.
 * @param[in] size   the size of the output buffer.
 *
 * @returns the number of bytes read.
 */
size_t mumble_buffer_read(mumble_buffer_t* buffer, uint8_t* output,
                          size_t size);

/**
 * Resize the buffer.
 *
 * @param[in] buffer a pointer to the buffer to resize.
 * @param[in] size   the new buffer size.
 *
 * @returns the resulting size of the buffer.
 */
size_t mumble_buffer_resize(mumble_buffer_t* buffer, size_t size);

#endif /* MUMBLE_BUFFER_H */
