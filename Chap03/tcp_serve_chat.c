/* tcp_serve_toupper.c */
#include"chap03.h"
#include<ctype.h>
#include <string.h>

int main()
{
#if defined(_WIN32)
	WSADATA d;

	if (WSAStarup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n");
		return (1);
	}
#endif
	printf("Configuring local address...\n");
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo * bind_address;
	getaddrinfo(0, "1234", &hints, &bind_address);

	printf("Creating socket...\n");
	SOCKET socket_listen;

	socket_listen = socket(bind_address->ai_family,
						   bind_address->ai_socktype,
						   bind_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return (1);
	}

	printf("Binding socket to local address...\n");

	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
		return (1);
	}
	freeaddrinfo(bind_address);

	printf("Listening...\n");
	if (listen(socket_listen, 10) < 0)
	{
		fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
		return (1);
	}

	fd_set master;
	FD_ZERO(&master);
	FD_SET(socket_listen, &master);
	SOCKET max_socket = socket_listen;

	printf("waiting for connections...\n");

	while (1)
	{
		fd_set reads;
		reads = master;

		if (select(max_socket + 1, &reads, 0, 0, 0) < 0)
		{
			fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
			return (1);
		}

		SOCKET i;

		for (i = 1; i <= max_socket; i++)
		{
			if (FD_ISSET(i, &reads))
			{
				if (i == socket_listen)
				{
					struct sockaddr_storage client_address;
					socklen_t client_len = sizeof(client_address);

					SOCKET socket_client;

					socket_client = accept(socket_listen,
										  (struct sockaddr *)&client_address,
										  &client_len);
					if (!ISVALIDSOCKET(socket_client))
					{
						fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
						return (1);
					}

					FD_SET(socket_client, &master);

					if (socket_client > max_socket)
						max_socket = socket_client;

					char address_buffer[100];
					getnameinfo((struct sockaddr *) &client_address,
								client_len, address_buffer
								, sizeof(address_buffer), 0, 0,
								NI_NUMERICHOST);
					printf("New connection from %s\n", address_buffer);
				}
				else
				{
					char read[1024];
					int bytes_received = recv(i, read, 1024, 0);

					if (bytes_received < 1)
					{
						FD_CLR(i, &master);
						CLOSESOCKET(i);
						continue;
					}

					SOCKET j;

					for (j = 1; j <= max_socket; ++j)
					{
						if (FD_ISSET(j, &master))
						{
							if (j == socket_listen || j == i)
								continue;
							else
								send(j, read, bytes_received, 0);
						}
					}
				}
			} //if FD_ISSET
		} // for i to max_socket
	} // while

	printf("Closing listening socket...\n");
	CLOSESOCKET(socket_listen);

#if defined(_WIN32)
	WSACleanup();
#endif

	printf("Finisehd\n");

	return (0);
}
