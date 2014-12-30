#include <stdio.h>
#include "server.h"

int
mumble_server_connect(mumble_server_t* server)
{

}

int
mumble_server_get_addr(mumble_server_t* server, struct sockaddr_storage* addr)
{
	int result;
	struct addrinfo *address, hints;
	char port_buffer[6];

#ifdef _WIN32
	if (_itoa_s(server->port, port_buffer, sizeof port_buffer, 10) != 0)
#else
	if (snprintf(port_buffer, sizeof port_buffer, "%u", server->port) < 0)
#endif
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
	else
	{
		char buffer[INET6_ADDRSTRLEN];

		for (struct addrinfo* ptr = address; ptr != NULL; ptr = ptr->ai_next)
		{
			printf("mumble_server_connect: host: %s",
				   inet_ntop(AF_INET,
							 &((struct sockaddr_in*)ptr->ai_addr)->sin_addr,
							 buffer,
							 sizeof buffer));
		}
	}

	return 0;
}
