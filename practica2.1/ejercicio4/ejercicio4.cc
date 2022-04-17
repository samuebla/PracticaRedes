#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>

#include <iostream>

void processConexion(int sd){
    struct sockaddr client_addr;
	socklen_t client_len = sizeof(struct sockaddr);

	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	int sd_client = accept(sd, &client_addr, &client_len);

	getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
				NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

	std::cout << "Conexión desde: " << host << " " << service
			  << std::endl;

	while (true)
	{
		// Tratamiento del mensaje
		char buffer[80];

		ssize_t bytes = recv(sd_client, buffer, sizeof(char) * 79, 0);
		if (bytes <= 0)
			break;

		// Volvemos a enviar de vuelta el mensaje
		if (send(sd_client, buffer, bytes, 0) == -1)
			break;
	}

	std::cout << "Conexión terminada" << std::endl;
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "No se ha dado la direccion"
				  << std::endl;
		return EXIT_FAILURE;
	}

	if (argc < 3)
	{
		std::cerr << "Se debe de dar el puerto"
				  << std::endl;
		return EXIT_FAILURE;
	}

	struct addrinfo hints;
	struct addrinfo *res;

	// Inicializamos el socket

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rc != 0)
	{
		std::cerr << "Error de getaddrinfo: " << gai_strerror(rc) << std::endl;
		return EXIT_FAILURE;
	}

	int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    //Bind

	if (bind(sd, res->ai_addr, res->ai_addrlen) != 0)
	{
		std::cerr << "Error de bind: " << std::endl;
		return EXIT_FAILURE;
	}

	// Libermaos los datos
	freeaddrinfo(res);

	// Comando listen
	if (listen(sd, 16) == -1)
	{
		std::cerr << "Error en listen." << std::endl;
		return EXIT_FAILURE;
	}

	// Aqui gestionamos las conexiones entrantes
    processConexion(sd);

	// Cierra el socket:
	close(sd);

	return EXIT_SUCCESS;
}