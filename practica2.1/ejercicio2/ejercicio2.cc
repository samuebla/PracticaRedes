#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

#define MAX_RESPONSE_LEN 15

int processingConnection(int sd, char buffer[80], struct sockaddr client_addr, socklen_t client_len,
						char host[NI_MAXHOST], char service[NI_MAXSERV]){
	time_t time_;
	struct tm *tm_;
	
	bool exit = false;
	char response[MAX_RESPONSE_LEN];
	
	//Bucle
	while (!exit)
	{
        //Tamaño
		ssize_t bytes = recvfrom(sd, buffer, 79 * sizeof(char), 0, &client_addr,
								 &client_len);

        //Por si hay error
		if (bytes == -1)
		{
			std::cerr << "recvfrom: " << std::endl;
			return EXIT_FAILURE;
		}


		getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
					NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

		std::cout << bytes << " bytes de " << host << ":" << service << std::endl;

        //Si bytes es 0...
		if (bytes == 0)
		{
            //Ignoramos el input y volvemos a comenzar el bucle
			continue;
		}

        //INPUT
		switch (buffer[0])
		{
        //Si pulsas la Q
		case 'q':
            //Salimos
			exit = true;
			std::cout << "Saliendo..." << std::endl;
			break;
		case 't':
			//Te devuelve la hora
			time(&time_);
			tm_ = localtime(&time_);
			bytes = strftime(response, MAX_RESPONSE_LEN, "%H:%M:%S %p", tm_);
			sendto(sd, response, bytes, 0, &client_addr, client_len);
			break;
		case 'd':
			//Te devuelve la fecha
			time(&time_);
			tm_ = localtime(&time_);
			bytes = strftime(response, MAX_RESPONSE_LEN, "%Y-%m-%d", tm_);
			sendto(sd, response, bytes, 0, &client_addr, client_len);
			break;
		default:
			std::cout << "Comando no soportado " << buffer[0] << std::endl;
		}
	}
	return 0;
}

int main(int, char **argv)
{
	struct addrinfo hints;
	struct addrinfo *res;

    //Iniciamos socket y bind

    //Guardamos memoria para hints del tamaño del struct
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

	//error
	if (rc != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
		return -1;
	}

	int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    //Si falla...
	if (bind(sd, res->ai_addr, res->ai_addrlen) != 0)
	{
		std::cerr << "bind: " << std::endl;
		return -1;
	}

	// Libera la información de la dirección una vez ya hemos usado sus datos
	freeaddrinfo(res);

    //Recepcion mensaje de cliente
	char buffer[80];
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	struct sockaddr client_addr;
	socklen_t client_len = sizeof(struct sockaddr);

	


	if(processingConnection(sd, buffer, client_addr, client_len,
						host, service) == 1){
		return EXIT_FAILURE;
	}
    

	//Cerramos el shocket al final
	close(sd);

	return EXIT_SUCCESS;
}