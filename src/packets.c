#include <stdio.h>

#include <mumble/server.h>
#include <mumble/channel.h>
#include <mumble/user.h>
#include "protocol.h"
#include "packets.h"
#include "log.h"
#include "iserver.h"
#include "Mumble.pb-c.h"

int mumble_packet_handle_ping(struct mumble_server_t* srv, const uint8_t* body,
                              uint32_t length)
{
    MumbleProto__Ping* ping = mumble_proto__ping__unpack(NULL, length, body);

    LOG_DEBUG("Received ping packet");

    mumble_proto__ping__free_unpacked(ping, NULL);

    return 1;
}

int mumble_packet_handle_crypt_setup(struct mumble_server_t* srv,
                                     const uint8_t* body, uint32_t length)
{
    MumbleProto__CryptSetup* crypt_setup =
        mumble_proto__crypt_setup__unpack(NULL, length, body);

    LOG_DEBUG("Received crypt setup packet");

    mumble_proto__crypt_setup__free_unpacked(crypt_setup, NULL);

    return 1;
}

int mumble_packet_handle_codec_version(struct mumble_server_t* srv,
                                       const uint8_t* body, uint32_t length)
{
    MumbleProto__CodecVersion* codec_version =
        mumble_proto__codec_version__unpack(NULL, length, body);

    LOG_DEBUG("Server codec (opus=%u)", codec_version->opus);

    mumble_proto__codec_version__free_unpacked(codec_version, NULL);
    return 1;
}

int mumble_packet_handle_server_sync(struct mumble_server_t* srv,
                                     const uint8_t* body, uint32_t length)
{
    MumbleProto__ServerSync* server_sync =
        mumble_proto__server_sync__unpack(NULL, length, body);

    if (server_sync->has_session)
        srv->session = server_sync->session;

    if (server_sync->has_max_bandwidth)
        srv->max_bandwidth = server_sync->max_bandwidth;

    if (server_sync->welcome_text != NULL)
        srv->welcome_text = strdup(server_sync->welcome_text);

    if (server_sync->has_permissions)
        srv->permissions = server_sync->permissions;

    LOG_DEBUG("Server synchronization complete (session=%d)", srv->session);
    LOG_DEBUG("Welcome text: %s", srv->welcome_text);

    mumble_proto__server_sync__free_unpacked(server_sync, NULL);

    return 1;
}

int mumble_packet_handle_channel_state(struct mumble_server_t* srv,
                                       const uint8_t* body, uint32_t length)
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
    {
        if (channel->name)
            free(channel->name);

        channel->name = strdup(channel_state->name);
    }

    if (channel_state->description != NULL)
    {
        if (channel->description)
            free(channel->description);

        channel->description = strdup(channel_state->description);
    }

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
int mumble_packet_handle_user_state(struct mumble_server_t* srv,
                                    const uint8_t* body, uint32_t length)
{
    int new_user = 0;
    mumble_user_t* user;
    MumbleProto__UserState* user_state =
        mumble_proto__user_state__unpack(NULL, length, body);

    if (!user_state->has_session)
    {
        LOG_WARN("Received user state doesn't have a session id");

        return 1;
    }

    for (user = srv->users; user != NULL; user = user->next)
    {
        if (user->session == user_state->session)
            break;
    }

    if (user == NULL)
    {
        /* Create a new user. */
        user = (mumble_user_t*)malloc(sizeof(mumble_user_t));
        mumble_user_init(user);
        user->next = srv->users;
        srv->users = user;
        new_user = 1;
    }

    if (user_state->has_session)
        user->session = user_state->session;

    if (user_state->has_actor)
        user->actor = user_state->actor;

    if (user_state->name != NULL)
    {
        if (user->name)
            free(user->name);

        user->name = strdup(user_state->name);
    }

    if (user_state->has_user_id)
        user->id = user_state->user_id;

    if (user_state->has_channel_id)
    {
        if (!new_user && user->channel_id != user_state->channel_id)
            LOG_DEBUG("User changed channel (channel %d -> channel %d)",
                      user->channel_id, user_state->channel_id);

        user->channel_id = user_state->channel_id;
    }

    if (user_state->has_mute)
    {
        if (user_state->mute)
            user->flags |= MUMBLE_USER_MUTE;
        else
            user->flags &= ~MUMBLE_USER_MUTE;
    }

    if (user_state->has_deaf)
    {
        if (user_state->deaf)
            user->flags |= MUMBLE_USER_DEAF;
        else
            user->flags &= MUMBLE_USER_DEAF;
    }

    if (user_state->has_suppress)
    {
        if (user_state->suppress)
            user->flags |= MUMBLE_USER_SUPPRESS;
        else
            user->flags &= ~MUMBLE_USER_SUPPRESS;
    }

    if (user_state->has_self_mute)
    {
        if (user_state->self_mute)
            user->flags |= MUMBLE_USER_SELF_MUTE;
        else
            user->flags &= MUMBLE_USER_SELF_MUTE;
    }

    if (user_state->has_self_deaf)
    {
        if (user_state->self_deaf)
            user->flags |= MUMBLE_USER_SELF_DEAF;
        else
            user->flags &= ~MUMBLE_USER_SELF_DEAF;
    }

    if (user_state->has_recording)
    {
        if (user_state->recording)
            user->flags |= MUMBLE_USER_RECORDING;
        else
            user->flags &= MUMBLE_USER_RECORDING;
    }

    if (user_state->comment != NULL)
    {
        if (user->comment)
            free(user->comment);

        user->comment = strdup(user_state->comment);
    }

    if (user_state->hash != NULL)
    {
        if (user->hash)
            free(user->hash);

        user->hash = strdup(user_state->hash);
    }

    if (user_state->has_channel_id)
        user->channel_id = user_state->channel_id;

    LOG_DEBUG("Received user state (session=%d name='%s' channel=%d)",
              user->session, user->name, user->channel_id);

    mumble_proto__user_state__free_unpacked(user_state, NULL);

    return 1;
}

int mumble_packet_handle_version(struct mumble_server_t* srv,
                                 const uint8_t* body, uint32_t length)
{
    MumbleProto__Version* version =
        mumble_proto__version__unpack(NULL, length, body);

    LOG_DEBUG("Received version message: %s - %s (%s)", version->release,
              version->os, version->os_version);

    mumble_proto__version__free_unpacked(version, NULL);

    return 1;
}
