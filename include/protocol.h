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

#pragma once
#ifndef __MUMBLE_PROTOCOL_H
#define __MUMBLE_PROTOCOL_H

enum mumble_packet_type
{
	MUMBLE_PACKET_VERSION               = 0,
	MUMBLE_PACKET_UDPTUNNEL             = 1,
	MUMBLE_PACKET_AUTHENTICATE          = 2,
	MUMBLE_PACKET_PING                  = 3,
	MUMBLE_PACKET_REJECT                = 4,
	MUMBLE_PACKET_SERVER_SYNC           = 5,
	MUMBLE_PACKET_CHANNEL_REMOVE        = 6,
	MUMBLE_PACKET_CHANNEL_STATE         = 7,
	MUMBLE_PACKET_USER_REMOVE           = 8,
	MUMBLE_PACKET_USER_STATE            = 9,
	MUMBLE_PACKET_BAN_LIST              = 10,
	MUMBLE_PACKET_TEXT_MESSAGE          = 11,
	MUMBLE_PACKET_PERMISSION_DENIED     = 12,
	MUMBLE_PACKET_ACL                   = 13,
	MUMBLE_PACKET_QUERY_USERS           = 14,
	MUMBLE_PACKET_CRYPT_SETUP           = 15,
	MUMBLE_PACKET_CONTEXT_ACTION_MODIFY = 16,
	MUMBLE_PACKET_CONTEXT_ACTION        = 17,
	MUMBLE_PACKET_USER_LIST             = 18,
	MUMBLE_PACKET_VOICE_TARGET          = 19,
	MUMBLE_PACKET_PERMISSION_QUERY      = 20,
	MUMBLE_PACKET_CODEC_VERSION         = 21,
	MUMBLE_PACKET_USER_STATS            = 22,
	MUMBLE_PACKET_REQUEST_BLOB          = 23,
	MUMBLE_PACKET_SERVER_CONFIG         = 24,
	MUMBLE_PACKET_SUGGEST_CONFIG        = 25
};

#endif /* __MUMBLE_PROTOCOL_H */
