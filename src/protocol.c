#include "protocol.h"
#include "Mumble.pb-c.h"

size_t mumble_packet_size_packed(mumble_packet_type_t packet_type,
								 const void* buffer)
{
	size_t size = 0;

	switch (packet_type)
	{
		case MUMBLE_PACKET_VERSION:
			size = mumble_proto__version__get_packed_size(buffer);

		case MUMBLE_PACKET_AUTHENTICATE:
			size = mumble_proto__authenticate__get_packed_size(buffer);

		default:
			return 0;
	}

	return size;
}
