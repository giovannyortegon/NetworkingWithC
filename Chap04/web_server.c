#include "chap04.h"

const char *get_content_type(const char * path);

SOCKET create_socket(const char * host, const char *port)
{
	printf("Configuring Local address...\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo * bind_address;
	getaddrinfo(host, port, &hints, &bind_address);

	printf("Creating socket...\n");
	SOCKET socket_listen;
	socket_listen = socket(bind_address->ai_family,
			bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		exit(1);
	}

	printf("Binding socket to local address...\n");
	if (bind(socket_listen,
				bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
	}
	freeaddrinfo(bind_address);

	printf("Listening...\n");
	if (listen(socket_listen, 10) < 0)
	{
		fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
		exit(1);
	}
	return socket_listen;
}

const char *get_content_type(const char * path)
{
	const char *last_dot = strrchr(path, '.');
	if (last_dot){
		if (strcmp(last_dot, ".css") == 0) return "text/css";
		if (strcmp(last_dot, ".csv") == 0) return "text/csv";
		if (strcmp(last_dot, ".gif") == 0) return "image/gif";
		if (strcmp(last_dot, ".htm") == 0) return "text/html";
		if (strcmp(last_dot, ".html") == 0) return "text/html";
		if (strcmp(last_dot, ".ico") == 0) return "image/x-icon";
		if (strcmp(last_dot, ".jpeg") == 0) return "image/jpeg";
		if (strcmp(last_dot, ".jpg") == 0) return "image/jpeg";
		if (strcmp(last_dot, ".js") == 0) return "application/javascript";
		if (strcmp(last_dot, ".json") == 0) return "application/json";
		if (strcmp(last_dot, ".png") == 0) return "image/png";
		if (strcmp(last_dot, ".pdf") == 0) return "application/pdf";
		if (strcmp(last_dot, ".svg") == 0) return "image/svg+xml";
		if (strcmp(last_dot, ".txt") == 0) return "text/plain";
	}
	return "application/octet-stream";
}
struct client_info *get_client(SOCKET s)
{
	struct client_info *ci = clients;

	while(ci)
	{
		if (ci->socket == s)
			break;
		ci = ci->next;
	}
	if (ci)
		return (ci);

	struct client_info *n = (struct client_info) calloc(1, sizeof(struct
				client_info));

	if (!n)
	{
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}

	n->address_length = sizeof(n->address);
	n->next = clients;
	clients = n;

	return (n);
}
void drop_client(struct client_info *client)
{
	CLOSESOCKET(client->socket);
	struct client_info **p = &clients;

	while(*p)
	{
		if (*p == client)
		{
			*p = client->next;
			free(client);
			return;
		}
		p = &(*p)->next;
	}
	fprintf(stderr, "drop_client not found.\n");
	exit(1);
}
