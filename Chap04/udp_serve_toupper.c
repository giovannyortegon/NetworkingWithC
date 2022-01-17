#include"chap04.h"
#include<ctype.h>

int main() {
#if defined(_WIN32)
    WSADATA d;

    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return (1);
    }
#endif
    printf("Configuring local address...\n");
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo * bind_address;

	getaddrinfo(0, "8080", &hints, &bind_address);

	printf("Creating socket...\n");
	SOCKET socket_listen;

	socket_listen = socket(bind_address->ai_family,
						   bind_address->ai_socktype,
						   bind_address->ai_protocol);

	if (!ISVALIDSOCKET(socket_listen)) {
		fprintf(stderr, "socket() failed.(%d)\n", GETSOCKETERRNO());
		return (1);
	}

	printf("Binding socket to local address...\n");

	if (bind(socket_listen,
		bind_address->ai_addr,
		bind_address->ai_addrlen)) {
		fprintf(stderr, "bind() failed. (%d) \n", GETSOCKETERRNO());

		return (1);
	}

	freeaddrinfo(bind_address);

	fd_set master;
	FD_ZERO(&master);
	FD_SET(socket_listen, &master);
	SOCKET max_socket = socket_listen;

	printf("Waiting for connections...\n");

	while (1) {
		fd_set reads;
		reads = master;

		if (select(max_socket + 1, &reads, 0, 0, 0) < 0) {
			fprintf(stderr, "select() failed.\n", GETSOCKETERRNO());
			return (1);
		}

		if (FD_ISSET(socket_listen, &reads)) {
			struct sockaddr_storage client_address;
			socklen_t client_len = sizeof(client_address);

			char read[1024];
			int bytes_received = recvfrom(socket_listen, read, 1024, 0,
										  (struct sockaddr *)&client_address,
										  &client_len);


			if (bytes_received < 1) {
				fprintf(stderr, "connect closed. (%d)\n", GETSOCKETERRNO());
				return (1);
			}

			int j;

			for (j = 0; j < bytes_received; ++j) {
				read[j] = toupper(read[j]);
			}

			sendto(socket_listen, read, bytes_received, 0,
				  (struct sockaddr *)&client_address, client_len);
		} // if
	} // while

	printf("Closing listening socket...\n");
	CLOSESOCKET(socket_listen);

#if defined(_WIN32)
	WSACleanup();
#endif
	printf("Fiished.\n");

	return (0);

}
