#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char * address, const char * port):sd(-1)
{
    //Construir un socket de tipo AF_INET y SOCK_DGRAM usando getaddrinfo.
    //Con el resultado inicializar los miembros sd, sa y sa_len de la clase
    struct addrinfo info;

    memset((void *)&info, 0, sizeof(struct addrinfo));

    info.ai_family = AF_INET;
    info.ai_socktype = SOCK_DGRAM;

    struct addrinfo *solution;

    int rc = getaddrinfo(address,port,&info,&solution);

    if (rc != 0)
        std::cerr << "Error al intentar obtener la información del address: " << gai_strerror(rc) << "\n";


    sd = socket(solution->ai_family, solution->ai_socktype, 0);

    if (sd == -1)
        std::cerr << "Error al inicializar el socket\n";
    

    sa = *solution->ai_addr;
    sa_len = solution->ai_addrlen;

    freeaddrinfo(solution);
}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    std::cout<<"Recibido mensaje\n";

    if ( bytes <= 0 )
    {
        std::cout<<"No creado socket\n";
        return -1;
    }

    if ( sock != 0 )
    {
        std::cout<<"Creado socket\n";
        sock = new Socket(&sa, sa_len);
        std::cout<<"Info socket: "<<*sock<<"\n";
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    //Serializar el objeto
    //Enviar el objeto binario a sock usando el socket sd
    obj.to_bin();
    std::cout<<"Enviando mensaje...\n";
    std::cout<<"Info socket: "<< sock <<"\n";
    ssize_t message = ::sendto(sd, obj.data(), obj.size(), 0, &sock.sa, sock.sa_len);
    if(message <= 0){
        std::cout << "No se pudo enviar el mensaje\n";
        return -1;
    }
    else return 0;
}

bool operator== (const Socket &s1, const Socket &s2)
{
    //Comparar los campos sin_family, sin_addr.s_addr y sin_port
    //de la estructura sockaddr_in de los Sockets s1 y s2
    //Retornar false si alguno difiere
    struct sockaddr_in *address1 = (struct sockaddr_in *) &(s1.sa);
    struct sockaddr_in *address2 = (struct sockaddr_in *) &(s2.sa);

    return (address1->sin_family == address2->sin_family && address1->sin_port == address2->sin_port && 
            address1->sin_addr.s_addr == address2->sin_addr.s_addr);
};

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv <<"\n";

    return os;
};
