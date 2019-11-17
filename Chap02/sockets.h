/* Sockets Library */
#ifndef SOCKETS_H
#define SOCKETS_H

#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0X0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "win2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif

#if !defined(_WIN32)
#define SOCKET int
#endif

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#endif

#if defined(_WIN32)
#define CLOSESOCKET(s) closesocket(s)
#else
#define CLOSESOCKET(s) close(s)
#endif

#if defined(_WIN32)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define GETSOCKETERRNO() (errno)
#endif

#endif /* HOLBERTON_H */
