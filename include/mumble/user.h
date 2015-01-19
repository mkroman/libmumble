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
#include <stdint.h>

/**
 * @file user.h
 * @author Mikkel Kroman
 * @date 16 Jan 2015
 * @brief User functions and data structures.
 */

#pragma once
#ifndef MUMBLE_USER_H
#define MUMBLE_USER_H

/**
 * Mumble user flags.
 */
typedef enum mumble_user_flags_t
{
    MUMBLE_USER_MUTE             = (1 << 1),
    MUMBLE_USER_DEAF             = (1 << 2),
    MUMBLE_USER_SUPPRESS         = (1 << 3),
    MUMBLE_USER_SELF_MUTE        = (1 << 4),
    MUMBLE_USER_SELF_DEAF        = (1 << 5),
    MUMBLE_USER_PRIORITY_SPEAKER = (1 << 6),
    MUMBLE_USER_RECORDING        = (1 << 7)
} mumble_user_flags_t;

/**
 * Mumble user structure.
 */
typedef struct mumble_user_t
{
    uint32_t id;
    uint32_t session;
    uint32_t actor;
    char* name;
    uint32_t channel_id;
    char* comment;
    char* hash;
    mumble_user_flags_t flags;
    struct mumble_user_t* next;
} mumble_user_t;

/**
 * Initialize a mumble user struct.
 */
mumble_user_t* mumble_user_init(mumble_user_t* user);

/**
 * Destroy a mumble user struct.
 */
void mumble_user_destroy(mumble_user_t* user);

#endif
