#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

	//Serializar los campos type, nick y message en el buffer _data
	char *tmp = _data;

    //Seriliazamos type
	memcpy(tmp, &type, sizeof(uint8_t));

	tmp += sizeof(uint8_t);

    //Serializamos el nick
	memcpy(tmp, nick.c_str(), 8 * sizeof(char));

	tmp += 8 * sizeof(char);

    //Serializamos el mensaje
	memcpy(tmp, message.c_str(), 80 * sizeof(char));

}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char *tmp = _data;

    //Reconstruccion de la clase
    memcpy(&type, tmp, sizeof(uint8_t));

    tmp += sizeof(uint8_t);

    //Guardamos los nuevos valores
    nick = tmp;
    tmp += sizeof(char) * 8;
    message = tmp;
    
    tmp += sizeof(char) * 80;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
        
        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el mensajeisor)

        ChatMessage message;
		Socket *cliente;

        //Recibimos el mensaje
		socket.recv(message, cliente);

		std::unique_ptr<Socket> clientPtrSocket(cliente);

        //Procesamos el mensaje dependiendo de su tipo
		switch (message.type)
		{
		case ChatMessage::LOGIN :
            std::cout << "Usuario conectado: " << message.nick <<  "\n";
            // Añadimos los clientes
            clients.push_back(std::move(clientPtrSocket));
			break;

		case ChatMessage::MESSAGE:
            //Enviamos el mensaje a todos los clientes
            std::cout << "Enviando mensaje a clientes...\n";
            for (auto &c : clients){
                std::cout<<"Procesando cliente: "<<*c.get()<<"\n";
                if (*c.get() == *clientPtrSocket){
                    std::cout << "Saltando cliente del mensaje...\n";
                    continue;
                }
                printf(message.nick.c_str());
                printf(message.message.c_str());
                std::cout << "Comenzamos transimision de mensaje..."<<*c<< "\n";
                socket.send(message, *c);
            }
			break;

		case ChatMessage::LOGOUT:
		{
            //Cierre de sesion
			bool encontrado = false;
			auto it = clients.begin();
			while (!encontrado && it != clients.end())
			{
				if (**it == *clientPtrSocket)
				{
					it = clients.erase(it);
                    std::cout << "Usuario desconectado: " << message.nick <<  "\n";
					encontrado = true;
				}
				else  it++;
			}
			break;
		}
        
		default:
            std::cerr << "Mensaje no reconocido\n";
			break;
		}
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage mensaje(nick, msg);
    mensaje.type = ChatMessage::LOGIN;

    socket.send(mensaje, socket);
}

void ChatClient::logout()
{
    // Completar
    std::string msg;

    ChatMessage mensaje(nick, msg);
    mensaje.type = ChatMessage::LOGOUT;

    socket.send(mensaje, socket);
}

void ChatClient::input_thread()
{
    while (c)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
        std::string msg;
        std::getline(std::cin, msg);

        ChatMessage mensaje(nick, msg);

        //Si el mensaje es para salir entonces mandamos un mensaje de salida, sino enviamos el mensaje
        if(msg == "exit"){
            mensaje.type = ChatMessage::LOGOUT;
            c = false;
        }
        else{
            mensaje.type = ChatMessage::MESSAGE;
            std::cout << "Mensaje: " << mensaje.message << " enviado con éxito" << std::endl;
        }

        socket.send(mensaje, socket);
    }
}

void ChatClient::net_thread()
{
    while(c)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        ChatMessage mensaje;

        //Recibimos el mensaje
        socket.recv(mensaje);

        //Escribimos su nick junto al  mensaje en cuestion
        std::cout << mensaje.nick << ": " << mensaje.message << std::endl;
    }
}
