#include <stdlib.h>
#include "channel.h"

mumble_channel_t* mumble_channel_init(mumble_channel_t* channel)
{
	channel->flags = 0;
	channel->name = NULL;
	channel->description = NULL;

	return channel;
}
