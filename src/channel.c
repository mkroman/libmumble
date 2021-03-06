#include <stdlib.h>
#include <mumble/channel.h>

mumble_channel_t* mumble_channel_init(mumble_channel_t* channel)
{
    channel->flags = 0;
    channel->name = NULL;
    channel->description = NULL;

    return channel;
}

void mumble_channel_free(mumble_channel_t* channel)
{
    if (!channel)
        return;

    free(channel->name);
    free(channel->description);
    free(channel);
}
