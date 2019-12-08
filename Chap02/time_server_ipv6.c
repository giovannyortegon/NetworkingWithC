/* time_server.c */
#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif
#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())

#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>

int main()
{
#if defined(_WIN32)
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n");
		return (1);
	}
#endif
/* Fill Protocols */
	printf("Configuring local address... \n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *bind_address;
	getaddrinfo(0, "8080", &hints, &bind_address);
/* Create Scoket */
	printf("Create socket... \n");
	SOCKET socket_listen;  /* win32 and Macro set up for Linux */
	socket_listen = socket(bind_address->ai_family,
			bind_address->ai_socktype, bind_address->ai_protocol);
/*Check Connection*/
	if (!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return (1);
	}
/* Bind associate wiht info from getaddress */
	printf("Binding socket to local address...\n");
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
		return (1);
	}
/* release the address memory */
	freeaddrinfo(bind_address);
/* Start to listening for connections */
	printf("Listening... \n");
	if (listen(socket_listen, 10) < 0)
	{
		fprintf(stderr, "listen() faild. (%d)\n", GETSOCKETERRNO());
		return (1);
	}
/* Accept any incoming connection */
	printf("Waiting for connection...\n");
/* Get info of client connection */
	struct sockaddr_storage client_address;
	socklen_t client_len = sizeof(client_address);

	SOCKET socket_client = accept(socket_listen,
			(struct sockaddr*) &client_address, &client_len);
	if (!ISVALIDSOCKET(socket_client))
	{
		fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
/* Print information about client connected */
	printf("Client is connected...");
	char address_buffer[100];
	getnameinfo((struct sockaddr*) &client_address,
			client_len, address_buffer, sizeof(address_buffer), 0, 0,
			NI_NUMERICHOST); /* NI_NUMERICHOST - return IP and hostname */
	printf("%s\n", address_buffer);

/* Recive a HTTP request */
	printf("Reading request...\n");
	char request[1024];
	int byte_received = recv(socket_client, request, 1024, 0);
	printf("Received %d bytes.\n", byte_received);

/* Response back with a timmer routine */
	printf("Sending response...\n");
	const char *response =
		"HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
		"Content-Type: text/plain\r\n\r\n"
		"Local time is: ";
	
	int bytes_sent = send(socket_client, response, strlen(response), 0);
	printf("Sent %d of %d bytes.\n", bytes_sent, (int) strlen(response));
/* Send to browser */
	time_t timer;
	time(&timer);
	char *time_msg = ctime(&timer);
	bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
	printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));
/* Close connections */
	printf("Closing connection...\n");
	CLOSESOCKET(socket_client);

	printf("Closing listening socket...\n");
	CLOSESOCKET(socket_listen);

#if defined(_WIN32)
	WSACleanup();
#endif
	printf("Finished.\n");
	return (0);
}
