#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>

#include <iostream>

void scan(char *address, char *port, struct addrinfo *hints, struct addrinfo *res, int ai_family)
{
	hints->ai_family = ai_family;

	struct addrinfo *hintsGroup[3];

	hintsGroup[0] = hints;
	hintsGroup[1] = hints;
	hintsGroup[2] = hints;

	hintsGroup[0]->ai_socktype = SOCK_STREAM;
	hintsGroup[1]->ai_socktype = SOCK_DGRAM;
	hintsGroup[2]->ai_socktype = SOCK_RAW;

	for (int i = 0; i < 3; ++i){
		int rc = getaddrinfo(address, port, hintsGroup[i], &res);

		if (rc != 0)
		{
			std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
			continue;
		}

		char host[NI_MAXHOST];
		char service[NI_MAXSERV];

		getnameinfo(res->ai_addr, res->ai_addrlen, host, NI_MAXHOST, service,
				NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

		std::cout << "IP: " << host << " " << res->ai_family << " " << res->ai_socktype << std::endl;
	}
}

int main(int, char **argv)
{
	struct addrinfo hints;
	struct addrinfo *res;

	memset(&hints, 0, sizeof(struct addrinfo));
	memset(&res, 0, sizeof(struct addrinfo));

	scan(argv[1], argv[2], &hints, res, AF_INET);
	scan(argv[1], argv[2], &hints, res, AF_INET6);

	// Libera los datos
	freeaddrinfo(res);

	return 0;
}