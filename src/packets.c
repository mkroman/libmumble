#include <stdio.h>

#include "server.h"
#include "packets.h"
#include "protocol.h"
#include "Mumble.pb-c.h"

int
mumble_packet_handle_crypt_setup(mumble_server_t* srv, const uint8_t* body,
								 uint32_t length)
{
	MumbleProto__CryptSetup* crypt_setup =
		mumble_proto__crypt_setup__unpack(NULL, length, body);

	printf("Received crypt setup packet\n");

	mumble_proto__crypt_setup__free_unpacked(crypt_setup, NULL);

	return 1;
}

int
mumble_packet_handle_codec_version(mumble_server_t* srv, const uint8_t* body,
								 uint32_t length)
{
	MumbleProto__CodecVersion* codec_version =
		mumble_proto__codec_version__unpack(NULL, length, body);

	printf("Server codec version: %u.%u %u\n", codec_version->alpha,
		   codec_version->beta, codec_version->opus);

	mumble_proto__codec_version__free_unpacked(codec_version, NULL);
	return 1;
}

int
mumble_packet_handle_server_sync(mumble_server_t* srv, const uint8_t* body,
								 uint32_t length)
{
	printf("Received server sync packet\n");

	return 1;
}

int
mumble_packet_handle_version(mumble_server_t* srv, const uint8_t* body,
								 uint32_t length)
{
	MumbleProto__Version* version =
			mumble_proto__version__unpack(NULL, length, body);

	printf("Received version message: %s - %s (%s)\n",
		   version->release, version->os, version->os_version);

	mumble_proto__version__free_unpacked(version, NULL);

	return 1;
}
