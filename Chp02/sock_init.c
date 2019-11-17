/* sock_init.c */
#include <stdio.h>
#include "sockets.h"

int main()
{
/* Code to Windows */
#if defined(_WIN32)
	WSADATA d;

	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n");
		return (1);
	}
#endif
	printf("Ready to use socket API.\n");
/* Code to Windows */
#if defined(_WIN32)
	WSACleanup();
#endif

	return (0);
}
