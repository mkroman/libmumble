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
 * @file packets.h
 * @author Mikkel Kroman
 * @date 15 Jan 2015
 * @brief Packet handlers for the different types of packets.
 */
#include <stdint.h>
#include <stdlib.h>

#include "protocol.h"

#pragma once
#ifndef MUMBLE_PACKETS_H
#define MUMBLE_PACKETS_H

#define MUMBLE_HANDLER_FUNC(name) \
	int mumble_packet_handle_##name(struct mumble_server_t* srv,  \
									 const uint8_t* body, \
									 uint32_t length)

MUMBLE_HANDLER_FUNC(version);
MUMBLE_HANDLER_FUNC(ping);
MUMBLE_HANDLER_FUNC(channel_state);
MUMBLE_HANDLER_FUNC(crypt_setup);
MUMBLE_HANDLER_FUNC(codec_version);
MUMBLE_HANDLER_FUNC(server_sync);

static mumble_handler_func_t g_mumble_packet_handlers[MUMBLE_PACKET_MAX] = {
	mumble_packet_handle_version, /* MUMBLE_PACKET_VERSION */
	NULL, /* MUMBLE_PACKET_UDPTUNNEL */
	NULL, /* MUMBLE_PACKET_AUTHENTICATE */
	mumble_packet_handle_ping, /* MUMBLE_PACKET_PING */
	NULL, /* MUMBLE_PACKET_REJECT */
	mumble_packet_handle_server_sync, /* MUMBLE_PACKET_SERVER_SYNC */
	NULL, /* MUMBLE_PACKET_CHANNEL_REMOVE */
	mumble_packet_handle_channel_state, /* MUMBLE_PACKET_CHANNEL_STATE */
	NULL, /* MUMBLE_PACKET_USER_REMOVE */
	NULL, /* MUMBLE_PACKET_USER_STATE */
	NULL, /* MUMBLE_PACKET_BAN_LIST */
	NULL, /* MUMBLE_PACKET_TEXT_MESSAGE */
	NULL, /* MUMBLE_PACKET_PERMISSION_DENIED */
	NULL, /* MUMBLE_PACKET_ACL */
	NULL, /* MUMBLE_PACKET_QUERY_USERS */
	mumble_packet_handle_crypt_setup, /* MUMBLE_PACKET_CRYPT_SETUP */
	NULL, /* MUMBLE_PACKET_CONTEXT_ACTION_MODIFY */
	NULL, /* MUMBLE_PACKET_CONTEXT_ACTION */
	NULL, /* MUMBLE_PACKET_USER_LIST */
	NULL, /* MUMBLE_PACKET_VOICE_TARGET */
	NULL, /* MUMBLE_PACKET_PERMISSION_QUERY */
	mumble_packet_handle_codec_version, /* MUMBLE_PACKET_CODEC_VERSION */
	NULL, /* MUMBLE_PACKET_USER_STATS */
	NULL, /* MUMBLE_PACKET_REQUEST_BLOB */
	NULL, /* MUMBLE_PACKET_SERVER_CONFIG */
	NULL  /* MUMBLE_PACKET_SUGGEST_CONFIG */
};

#endif
