#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "mumble.h"
#include "server.h"

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
	server->buffer[0] = 0;
	server->buffer_pos = 0;

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

	ev_io_init(&server->watcher, mumble_server_handshake, fd, EV_READ | EV_WRITE);
	ev_io_start(context->loop, &server->watcher);

	return result;
}

void mumble_server_callback(EV_P_ ev_io *w, int revents)
{
	if (revents & EV_WRITE)
	{
		/* Write any pending data. */

	}
	else /* Assume EV_READ. */
	{
		printf("mumble_server_read\n");
	}
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
