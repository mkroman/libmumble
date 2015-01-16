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
 * @file channel.h
 * @author Mikkel Kroman
 * @date 16 Jan 2015
 * @brief Channel functions and data structures.
 */
#include <stdint.h>

#pragma once
#ifndef MUMBLE_CHANNEL_H
#define MUMBLE_CHANNEL_H

typedef enum mumble_channel_flags_t
{
	MUMBLE_CHANNEL_TEMPORARY = (1 << 1)
} mumble_channel_flags_t;

typedef struct mumble_channel_t
{
	int id;
	int parent;
	char* name;
	char* description;
	int position;
	mumble_channel_flags_t flags;
	struct mumble_channel_t* next;
} mumble_channel_t;

/**
 * Initialize a new mumble channel structure.
 *
 * @param[in] channel the channel to initialize.
 *
 * @returns a pointer to the channel.
 */
mumble_channel_t* mumble_channel_init(mumble_channel_t* channel);

/**
 * Destroy a mumble channel structure.
 */
void mumble_channel_destroy(mumble_channel_t* channel);

#endif
