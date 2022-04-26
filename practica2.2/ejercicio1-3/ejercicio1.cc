#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
private:
    int16_t pos_x;
    int16_t pos_y;
    
    static const size_t MAX_NAME = 20;

    char name[MAX_NAME];
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):pos_x(_x),pos_y(_y)
    {
        //Guarda en la variable Name el nombre que le pongas en _n
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    int16_t getPos_X() { return pos_x;}
    int16_t getPos_Y() { return pos_y;}
    char* getName(){ return name;}


    //Serializar
    void to_bin()
    {
        //  Guardamos el tamaño necesario (2 int y 1 array de chars)
		_size = sizeof(int16_t) * 2 + sizeof(char) * MAX_NAME;

        //Reserva la memoria
		alloc_data(_size);

        //Cogemos el puntero
		char *currentPos = _data;

		//  Guardamos el nombre
		memcpy(currentPos, name, MAX_NAME * sizeof(char));

        //Pasamos a pos_x
		currentPos += MAX_NAME * sizeof(char);

		//Guardamos pos_x
		memcpy(currentPos, &pos_x, sizeof(int16_t));

        //Avanzamos a pos_y
		currentPos += sizeof(int16_t);

		//Guardamos pos_y
		memcpy(currentPos, &pos_y, sizeof(int16_t));
		currentPos += sizeof(int16_t);
    }

    //Deserializar
    int from_bin(char * data)
    {
       //Lo ponemos apuntando a la direccion de memoria
        char *currentPos = data;

        //Copiamos el nombre y lo guardamos en name
        memcpy(name, currentPos, MAX_NAME * sizeof(char));

        //Ahora el puntero avanza a pos_x
        currentPos += MAX_NAME * sizeof(char);

        //Copiamos la posicion de x
        memcpy(&pos_x, currentPos, sizeof(int16_t));

        //Avanzamos para llegar a pos_y
        currentPos += sizeof(int16_t);

        //Y copiamos pos_y
        memcpy(&pos_y, currentPos, sizeof(int16_t));

        return 0;
    }
};

int main(int argc, char **argv)
{
    //Read
    Jugador one_r("", 0, 0);
    //Write
    Jugador one_w("Player_One", 12, 345);



    // 1. Serializar el objeto one_w
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    //0666 es permisos de lectura escritura y ejecucion
    int lect = open("./data_jugador.txt", O_CREAT| O_TRUNC |O_RDWR, 0666);
    //Escribe en lect los datos de one_w
    ssize_t tam = write(lect,one_w.data(), one_w.size());

    //Si no ocupa el tamaño de one_w
    if(tam != one_w.size()){    //ERROR
        std::cout << "No se pudo escribir correctamente el fichero de player one\n";
        return -1;
    }
    //Y cerramos el flujo
    close(lect);

    // 3. Leer el fichero
    lect = open("./data_jugador.txt", O_RDONLY);
    //Guardamos en el buffer lo que leemos
    char buffer[tam];

    //Y si ha habido algun error...
    if(read(lect, &buffer, tam) == -1){
        std::cerr << "No se pudo leer el fichero del player one\n";
        return -1;
    }
    //Vuelvo a cerrar el flujo
    close(lect);
    
    // 4. "Deserializar" en one_r
    one_r.from_bin(buffer);

    // 5. Mostrar el contenido de one_r
    std::cout << "Nombre del jugador: " << one_r.getName() << "\n Posicion: (" << one_r.getPos_X() << ","  << one_r.getPos_Y() << ")" << std::endl;

    return 0;
}

