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
 * @file user.h
 * @author Mikkel Kroman
 * @date 16 Jan 2015
 * @brief User functions and data structures.
 */

#pragma once
#ifndef MUMBLE_USER_H
#define MUMBLE_USER_H

#include <stdint.h>

#include <mumble/external.h>

#ifdef __cplusplus
extern "C" {
#endif

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
 *
 * @param[in] user a pointer to a struct to initialize.
 *
 * @returns pointer to the user struct.
 */
MUMBLE_API mumble_user_t* mumble_user_init(mumble_user_t* user);

/**
 * Free all memory used by a user struct.
 *
 * @param[in] user a pointer to the user.
 */
MUMBLE_API void mumble_user_free(mumble_user_t* user);

#ifdef __cplusplus
}
#endif

#endif
