#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include "mumble.h"
#include "server.h"
#include "protocol.h"
#include "Mumble.pb-c.h"

int setnonblock(socket_t fd)
{
#ifdef __unix__
	return fcntl(fd, F_SETFL, O_NONBLOCK);
#endif /* __unix__ */
}

socket_t
mumble_server_create_socket()
{
	socket_t fd;

	fd = socket(PF_INET, SOCK_STREAM, 0);

	if (fd < 0)
	{
		fprintf(stderr, "mumble_server_create_socket: socket() failed\n");

		return -1;
	}
	
	if (setnonblock(fd) != 0)
	{
		fprintf(stderr, "mumble_server_create_socket: could not make socket non-blocking\n");

		return -1;
	}

	return fd;
}

int
mumble_server_init(mumble_server_t* server)
{
	if (!server)
		return 1;

	server->host = 0;
	server->read_buffer.data[0] = 0;
	server->read_buffer.pos = 0;
	server->read_buffer.size = 0;
	server->write_buffer.data[0] = 0;
	server->write_buffer.pos = 0;
	server->write_buffer.size = 0;

	return 0;
}

int
mumble_server_connect(mumble_server_t* server, struct mumble_t* context)
{
	int result;
	socket_t fd;
	struct addrinfo *address, hints;
	char port_buffer[6];

#ifdef _WIN32
	if (_itoa_s(server->port, port_buffer, sizeof port_buffer, 10) != 0)
#else
	if (snprintf(port_buffer, sizeof port_buffer, "%u", server->port) < 0)
#endif /* _WIN32 */
	{
		fprintf(stderr, "mumble_server_connect: could not convert port to number\n");

		return 1;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

	result = getaddrinfo(server->host, port_buffer, &hints, &address);

	if (result != 0)
	{
		fprintf(stderr, "mumble_server_connect: %s\n", gai_strerror(result));

		return 1;
	}

	fd = mumble_server_create_socket();
	result = 1;

	for (struct addrinfo* ptr = address; ptr != NULL; ptr = ptr->ai_next)
	{
		if (ptr->ai_family != AF_UNSPEC &&
			(connect(fd, (struct sockaddr*)ptr->ai_addr, ptr->ai_addrlen) != 0))
		{
			if (errno == EINPROGRESS)
			{
				// Connection status is not yet determined.
				result = 0;

				break;
			}
		}
		else
		{
			result = 0;

			break;
		}
	}

	freeaddrinfo(address);

	if (result != 0)
	{
		fprintf(stderr, "mumble_server_connect: connection failed\n");

		return 1;
	}

	server->fd = fd;

	// Set up SSL.
	server->ssl = SSL_new(context->ssl_ctx);

	if (server->ssl == NULL)
	{
		fprintf(stderr, "mumble_server_connect: could not create SSL object\n");

		return 1;
	}

	if (!SSL_set_fd(server->ssl, server->fd))
	{
		fprintf(stderr, "could not set file descriptor\n");

		return 1;
	}

	if ((result = SSL_connect(server->ssl)) != -1)
	{
		fprintf(stderr, "SSL_connect: %d (%d)\n", result,
				SSL_get_error(server->ssl, result));

		return 1;
	}

	server->watcher.data = server;

	ev_io_init(&server->watcher,
			   mumble_server_handshake, fd, EV_READ | EV_WRITE);
	ev_io_start(context->loop, &server->watcher);

	return result;
}

void mumble_server_callback(EV_P_ ev_io *w, int revents)
{
	int result;
	uint16_t type;
	uint32_t length;
	mumble_server_t* srv = (mumble_server_t*)w->data;

	if (revents & EV_WRITE)
	{
		/* Write any pending data. */
		
	}
	else /* Assume EV_READ. */
	{
		result = SSL_read(srv->ssl, (srv->read_buffer.data +
									 srv->read_buffer.pos), 512);

		if (result > 0)
		{
			srv->read_buffer.pos += (size_t)result;
			srv->read_buffer.size += (size_t)result;

			if (srv->read_buffer.size > kMumbleHeaderSize)
			{
				type = ntohs(*(uint16_t*)srv->read_buffer.data);
				length = ntohl(*(uint32_t*)(srv->read_buffer.data + sizeof(uint16_t)));

				if (srv->read_buffer.size >= (length + kMumbleHeaderSize))
					mumble_server_read_message(srv, type, length);
			}
		}
		else
		{
			fprintf(stderr, "SSL_read failed: %d\n", result);
		}
	}
}

int mumble_server_read_message(mumble_server_t* server, uint16_t type, uint32_t length)
{
	int message_size = kMumbleHeaderSize + length;
	const uint8_t* data =
		(const uint8_t*)server->read_buffer.data + kMumbleHeaderSize;

	if (server->read_buffer.size < message_size)
		return 0;

	switch (type)
	{
		case MUMBLE_PACKET_VERSION:
			{
				MumbleProto__Version* version =
					mumble_proto__version__unpack(NULL, length, data);

				printf("Received version message: %s - %s (%s)\n",
					   version->release, version->os, version->os_version);

				mumble_proto__version__free_unpacked(version, NULL);
				break;
			}
	}

	return 1;
}

void mumble_server_handshake(EV_P_ ev_io *w, int revents)
{
	mumble_server_t* srv = (mumble_server_t*)w->data;
	int result = SSL_do_handshake(srv->ssl);

	if (result == 1)
	{
		/* SSL handshake complete */
		LOG("mumble client handshake complete\n");

		/* Change the callback to the generic, non-handshake one. */
		ev_io_stop(EV_A_ w);
		ev_set_cb(w, mumble_server_callback);
		ev_io_set(w, srv->fd, EV_READ);
		ev_io_start(EV_A_ w);
	}
	else
	{
		int err = SSL_get_error(srv->ssl, result);

		if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
		{
			fprintf(stderr, "Unexpected SSL error during handshake: %d\n", err);
		}
	}
}

int mumble_server_send_version(mumble_server_t* server)
{
	MumbleProto__Version version = MUMBLE_PROTO__VERSION__INIT;

	version.version = 66055;
	version.release = "1.2.8";
	version.os = "X11";
	version.os_version = "Linux";



	return 0;
}
