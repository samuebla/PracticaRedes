#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define MAX_RESPONSE_LEN 15

int sendMessage(char** argv, int sd){
    char buffer[MAX_RESPONSE_LEN];

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	sendto(sd, argv[3], 2, 0, (struct sockaddr *)&server_addr,
				 sizeof(server_addr));

	socklen_t server_addr_len;
	ssize_t bytes = recvfrom(sd, buffer, (MAX_RESPONSE_LEN - 1) * sizeof(char), 0,
													 (struct sockaddr *)&server_addr, &server_addr_len);

    if (bytes == -1)
	{
		std::cerr << "recvfrom: " << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << buffer << std::endl;

    return 0;
}

int main(int, char **argv)
{
	struct addrinfo hints;
	struct addrinfo *res;

	//Iniciamos el socket

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rc != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
		return EXIT_FAILURE;
	}

	int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// libera memoria
	freeaddrinfo(res);

	// enviamos el mensaje
	if (sendMessage(argv, sd) == 1){
        return EXIT_FAILURE;
    }

	// Cerramos
	close(sd);

	return EXIT_SUCCESS;
}