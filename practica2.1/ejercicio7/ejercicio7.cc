#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <thread>

#include <iostream>

//Se encarga de tratar la conexión con el cliente
class Conexion
{
	int fd_;

public:
	Conexion(int fd) : fd_(fd) {}
	~Conexion() { close(fd_); }

	void run() const noexcept
	{
		// Bucle de recepción de mensajes
		while (true)
		{
			// Creamos un buffer para almacenar el mensaje
			char buffer[80];

			ssize_t bytes = recv(fd_, buffer, sizeof(char) * 79, 0);
			if (bytes <= 0)
				break;

			// Volvemos a mandarle su mismo mensaje de vuelta
			if (send(fd_, buffer, bytes, 0) == -1)
				break;
		}
	}
};

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Direccion no especificada"
				  << std::endl;
		return EXIT_FAILURE;
	}

	if (argc < 3)
	{
		std::cerr << "Puerto no especificado"
				  << std::endl;
		return EXIT_FAILURE;
	}

	struct addrinfo hints;
	struct addrinfo *res;

	// Inicialización

    //Guardamos la memoria
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	//Conseguimos la información necesaria
	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rc != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
		return EXIT_FAILURE;
	}

    //Abrimos el socket
	int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    //Y lo bindeamos
	if (bind(sd, res->ai_addr, res->ai_addrlen) != 0)
	{
		std::cerr << "bind: " << std::endl;
		return EXIT_FAILURE;
	}

	freeaddrinfo(res);

	// Escuchamos mensajes que vengan de fuera
	if (listen(sd, 16) == -1)
	{
		std::cerr << "No se ha podido establecer listen" << std::endl;
		return EXIT_FAILURE;
	}

	// Gestión de clientes
	struct sockaddr client_addr;
	socklen_t client_len = sizeof(struct sockaddr);

	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	while (true)
	{
		// Se queda esperando a conexiones
		int sd_client = accept(sd, &client_addr, &client_len);

		// Escribe la información del cliente
		getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
					NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

		std::cout << "Ip conectada: " << host << " " << service
				  << std::endl;

		// Crea la conexion y la elimina al finalizar
		const auto *connection = new Conexion(sd_client);
		std::thread([connection]() {
            connection->run(); delete connection;

			std::cout << "Conexión terminada." << std::endl;
		}).detach();
	}

	// Cerramos el socket
	close(sd);

	return EXIT_SUCCESS;
}