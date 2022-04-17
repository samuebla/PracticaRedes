#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <iostream>

#define MAX_RESPONSE_LEN 15

//Clase encargada de realizar las funciones de conexión entre servidor y cliente
class Conexion
{
	int fd_;

public:
	Conexion(int fd) : fd_(fd) {}

	void run() const noexcept
	{
		// Recibimos los mensajes continuamente
		time_t time_;
		struct tm *tm_;

		char buffer[80];
		char host[NI_MAXHOST];
		char service[NI_MAXSERV];

		struct sockaddr client_addr;
		socklen_t client_len = sizeof(struct sockaddr);

		// Escribimos la info del thread
		std::cout << "Numero de thread [" << std::this_thread::get_id() << "]" << std::endl;

		bool exit = false;
		char response[MAX_RESPONSE_LEN];
		while (!exit)
		{
			// Recibimos un mensaje
			ssize_t bytes = recvfrom(fd_, buffer, 79 * sizeof(char), 0, &client_addr,
									 &client_len);

			// Comprobamos si recvfrom ha dado error
			if (bytes == -1)
			{
				std::cerr << "Numero de thread [" << std::this_thread::get_id() << "]: "
						  << "error de recvfrom: " << std::endl;
				break;
			}

			// Añadimos una pequeña espera:
			std::this_thread::sleep_for(std::chrono::seconds(1));

			// Escribe algo de información del cliente:
			getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
						NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

			std::cout << "Numero de thread [" << std::this_thread::get_id() << "]: "
					  << bytes << " bytes recibidos de " << host << ":" << service << std::endl;

			// Si no se mandaron suficientes bytes, continua:
			if (bytes == 0)
			{
				continue;
			}

			switch (buffer[0])
			{
			case 't':
				time(&time_);
				tm_ = localtime(&time_);
				bytes = strftime(response, MAX_RESPONSE_LEN, "%H:%M:%S %p", tm_);
				sendto(fd_, response, bytes, 0, &client_addr, client_len);
				break;
			case 'd':
				time(&time_);
				tm_ = localtime(&time_);
				bytes = strftime(response, MAX_RESPONSE_LEN, "%Y-%m-%d", tm_);
				sendto(fd_, response, bytes, 0, &client_addr, client_len);
				break;
			default:
				std::cout << "Numero de thread [" << std::this_thread::get_id() << "]: "
						  << "Comando no soportado " << buffer[0] << std::endl;
			}
		}

		std::cout << "Numero de thread [" << std::this_thread::get_id() << "]: Sin actividad." << std::endl;
	}
};

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Error: no se ha especificado la direccion"
				  << std::endl;
		return EXIT_FAILURE;
	}

	if (argc < 3)
	{
		std::cerr << "Error: no se ha especificado puerto"
				  << std::endl;
		return EXIT_FAILURE;
	}

    if (argc < 4)
	{
		std::cerr << "Error: no se ha especificado el numero de threads"
				  << std::endl;
		return EXIT_FAILURE;
	}

	struct addrinfo hints;
	struct addrinfo *res;

    //Guardamos memoria
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	//Obtenemos la informacion necesaria
	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rc != 0)
	{
		std::cerr << "Error en getaddrinfo: " << gai_strerror(rc) << std::endl;
		return EXIT_FAILURE;
	}

	int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (bind(sd, res->ai_addr, res->ai_addrlen) != 0)
	{
		std::cerr << "Error en bind: " << std::endl;
		return EXIT_FAILURE;
	}

	freeaddrinfo(res);

	// Crea los threads especificados
	int n_threads = atoi(argv[3]);
	for (int i = 0; i < n_threads; ++i)
	{
		// Crea la conexión y la elimina al finalizar
		const auto *conn = new Conexion(sd);
		std::thread([conn]() { conn->run(); delete conn;}).detach();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	std::cout << "Se han creado " << n_threads << " threads" << std::endl;

	// Cerramos si se escribe q
	std::string v;
	do
	{
		std::cin >> v;
	} while (v != "q");

	// Cerramos el socket
	close(sd);

	return EXIT_SUCCESS;
}