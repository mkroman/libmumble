#include <stdio.h>

#include "log.h"
#include "server.h"
#include "packets.h"
#include "protocol.h"
#include "channel.h"
#include "Mumble.pb-c.h"

int
mumble_packet_handle_ping(mumble_server_t* srv, const uint8_t* body,
								 uint32_t length)
{
	MumbleProto__Ping* ping =
		mumble_proto__ping__unpack(NULL, length, body);

	MUMBLE_LOG("Received ping packet");

	mumble_proto__ping__free_unpacked(ping, NULL);

	return 1;
}

int
mumble_packet_handle_crypt_setup(mumble_server_t* srv, const uint8_t* body,
								 uint32_t length)
{
	MumbleProto__CryptSetup* crypt_setup =
		mumble_proto__crypt_setup__unpack(NULL, length, body);

	MUMBLE_LOG("Received crypt setup packet");

	mumble_proto__crypt_setup__free_unpacked(crypt_setup, NULL);

	return 1;
}

int
mumble_packet_handle_codec_version(mumble_server_t* srv, const uint8_t* body,
								 uint32_t length)
{
	MumbleProto__CodecVersion* codec_version =
		mumble_proto__codec_version__unpack(NULL, length, body);

	MUMBLE_LOG("Server codec (opus=%u)", codec_version->opus);

	mumble_proto__codec_version__free_unpacked(codec_version, NULL);
	return 1;
}

int
mumble_packet_handle_server_sync(mumble_server_t* srv, const uint8_t* body,
								 uint32_t length)
{
	MUMBLE_LOG("Received server sync packet");

	return 1;
}

int
mumble_packet_handle_channel_state(mumble_server_t* srv, const uint8_t* body,
								   uint32_t length)
{
	mumble_channel_t* channel = NULL;
	MumbleProto__ChannelState* channel_state =
		mumble_proto__channel_state__unpack(NULL, length, body);

	if (!channel_state->has_channel_id)
	{
		LOG_WARN("Received channel state that didn't contain a channel id");
		return 1;
	}

	for (channel = srv->channels; channel != NULL; channel = channel->next)
	{
		if (channel->id == (int)channel_state->channel_id)
			break;
	}

	if (channel == NULL)
	{
		channel = (mumble_channel_t*)malloc(sizeof(mumble_channel_t));
		mumble_channel_init(channel);

		channel->id = channel_state->channel_id;
		channel->next = srv->channels;

		srv->channels = channel;

		LOG_DEBUG("Created new channel");
	}

	if (channel_state->has_parent)
		channel->parent = channel_state->parent;

	if (channel_state->name != NULL)
		channel->name = strdup(channel_state->name);

	if (channel_state->description != NULL)
		channel->description = strdup(channel_state->description);

	if (channel_state->has_position)
		channel->position = channel_state->position;

	if (channel_state->has_temporary)
	{
		if (channel_state->temporary)
			channel->flags |= MUMBLE_CHANNEL_TEMPORARY;
		else
			channel->flags &= ~MUMBLE_CHANNEL_TEMPORARY;
	}

	LOG_DEBUG("Received channel state for channel (id=%d name='%s')",
			  channel->id, channel->name);

	mumble_proto__channel_state__free_unpacked(channel_state, NULL);

	return 1;
}

int
mumble_packet_handle_version(mumble_server_t* srv, const uint8_t* body,
								 uint32_t length)
{
	MumbleProto__Version* version =
			mumble_proto__version__unpack(NULL, length, body);

	MUMBLE_LOG("Received version message: %s - %s (%s)",
			   version->release, version->os, version->os_version);

	mumble_proto__version__free_unpacked(version, NULL);

	return 1;
}
