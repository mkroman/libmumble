#include <netdb.h>
#include <stdio.h>
#include "server.h"

int mumble_server_connect( mumble_server_t* server )
{
	int result;
	struct addrinfo *addressInfo, addressHints;
	char portBuffer[ 6 ];

#ifdef _WIN32
	if ( _itoa_s( server->port, portBuffer, sizeof portBuffer, 10 ) != 0 )
#else
	if ( snprintf( portBuffer, sizeof portBuffer, "%u", server->port ) < 0 )
#endif
	{
		fprintf( stderr, "mumble_server_connect: could not convert port to number\n" );

		return 1;
	}

	memset( &addressHints, 0, sizeof addressHints );
	addressHints.ai_family = AF_UNSPEC;
	addressHints.ai_protocol = IPPROTO_TCP;
	addressHints.ai_socktype = SOCK_STREAM;

	result = getaddrinfo( server->host, portBuffer, &addressHints, &addressInfo );

	if ( result != 0 )
	{
		fprintf( stderr, "mumble_server_connect: %d\n", result );

		return 1;
	}
	else
	{
		char addressBuffer[ 128 ];

		for ( struct addrinfo* ptr = addressInfo; ptr != NULL; ptr = ptr->ai_next )
		{
			printf( "mumble_server_connect: host: %s", inet_ntop( AF_INET, &( ( struct sockaddr_in* )ptr->ai_addr )->sin_addr, addressBuffer, sizeof addressBuffer ) );
		}
	}

	return 0;
}
