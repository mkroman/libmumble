#include <assert.h>
#include <stdio.h>

#include "protocol.h"
#include "Mumble.pb-c.h"

size_t mumble_packet_size_packed(mumble_packet_type_t packet_type,
                                 const void* buffer)
{
    size_t size = 0;

    switch (packet_type)
    {
        case MUMBLE_PACKET_PING:
            size = mumble_proto__ping__get_packed_size(buffer);
            break;
        case MUMBLE_PACKET_VERSION:
            size = mumble_proto__version__get_packed_size(buffer);
            break;
        case MUMBLE_PACKET_AUTHENTICATE:
            size = mumble_proto__authenticate__get_packed_size(buffer);
            break;
        default:
            assert(0 && "unknown packet type");
            size = 0;
    }

    return size;
}

size_t mumble_packet_proto_pack(mumble_packet_type_t packet_type, void* message,
                                void* buffer)
{
    size_t result;

    switch (packet_type)
    {
        case MUMBLE_PACKET_PING:
            result = mumble_proto__ping__pack(message, buffer);
            break;
        case MUMBLE_PACKET_VERSION:
            result = mumble_proto__version__pack(message, buffer);
            break;
        case MUMBLE_PACKET_AUTHENTICATE:
            result = mumble_proto__authenticate__pack(message, buffer);
            break;
        default:
            assert(0 && "unknown packet type");
            result = 0;
    }

    return result;
}
