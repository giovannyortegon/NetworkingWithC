#include <stdio.h>
#include"chap05.h"

int main() {
	char host[100];
	char serv[100];
	struct addrinfo hints;
	struct addrinfo *peer_address;
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_ALL;

	if (getaddrinfo("example.com", 0, &hints, &peer_address)) {
		fprintf(stderr, "getaddrinfo() failed. (%d)\n",
				GETSOCKETERRNO());
		return 1;
	}

	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
				host, sizeof(host),
				serv, sizeof(serv),
				NI_NAMEREQD | NI_NUMERICSERV);
	printf("%s %s\n", host, serv);

	freeaddrinfo(peer_address);
	return (0);
}
