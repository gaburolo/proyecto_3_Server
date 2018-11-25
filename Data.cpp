//
// Created by oldboy on 26/11/18.
//


#include <SFML/Network/SocketSelector.hpp>
#include "Data.h"

///Metodo para crear la tabla de la base de datos
void DataBase::createTable() {
    std::string content = "filename;extension;size;part1;part2;part3;parity/";
    send(content, "save");
}

/// Metodo para agregar un nuevo elemento a la tabla
/// \param content Elemento a agregar a la tabla
void DataBase::addToTable(std::string content) {
    std::string dataBase = getTable();
    dataBase.append(content);
    dataBase.append("/");
    send(dataBase, "save");
}

/// Metodo para obtener las partes de la tabla almacenadas en los discos y reconstruir la tabla
/// \return String con la tabla
std::string DataBase::getTable() {
    std::string part1 = "";
    std::string part2 = "";
    std::string part3 = "";
    std::string parity = "";

    //Se solicita a cada uno de los discos la parte de la tabla que almacenan
    for (int i = 0; i < 4; ++i) {
        sf::Packet packet;
        packet << "database";
        packet << "get";

        sf::TcpSocket *disk = Singleton::getDisks()->getElement(i)->getData();
        disk->send(packet);

        //Se espera la respuesta del disco
        sf::Packet receivePacket;
        std::string receiveMessage;
        if (disk->receive(receivePacket) == sf::Socket::Done) {
            receivePacket >> receiveMessage;
            if(i == 0){
                part1 = receiveMessage;
            } else if(i == 1){
                part2 = receiveMessage;
            } else if(i == 2){
                part3 = receiveMessage;
            } else if(i == 3){
                parity = receiveMessage;
            }
        }
    }

    //Casos en los que uno de los discos tuvo un fallo
    if(part1.size() == 0){
        part1 = Tolerancia::recoverData(part2, part3, parity);
    } else if(part2.size() == 0){
        part2 = Tolerancia::recoverData(part1, part3, parity);
    } else if(part3.size() == 0){
        part3 = Tolerancia::recoverData(part1, part2, parity);
    }

    part1 += (part2 + part3);

    return part1;
}

/// Metodo para almacenar la tabla en los discos
/// \param content Tabla
/// \param action Accion a realizar
void DataBase::send(std::string content, std::string action) {
    int size = content.size() / 3; //Se calcula el tamaño de cada parte

    int start = 0;
    int end = size;

    LinkedList<std::vector<byte>> parts = LinkedList<std::vector<byte>>(); //Lista en la que se guardan las partes para el calculo de la paridad

    for (int i = 0; i < 3; ++i) {
        if(i == 2)
            end = content.size();

        //Se divide el string en partes iguales
        std::string string = std::string(&content[start], &content[end]);
        std::vector<byte> vector(string.begin(), string.end());
        parts.insertAtEnd(vector);

        //Se calcula el nuevo inicio y el nuevo final de la siguiente parte
        start = end;
        end += size;

        //Se envia al disco
        sf::Packet packet;
        packet << "database";
        packet << action;
        packet << string;
        Singleton::getDisks()->getElement(i)->getData()->send(packet);
    }

    //Se realiza el calculo de la paridad
    std::vector<byte> parityVector = Tolerancia::calculateParity(parts.getElement(0)->getData(),
                                                                     parts.getElement(1)->getData(),
                                                                     parts.getElement(2)->getData());

    //Se envia la patidad al cuarto disco
    sf::Packet parity;
    parity << "database";
    parity << action;
    parity << std::string(parityVector.begin(), parityVector.end());
    Singleton::getDisks()->getElement(3)->getData()->send(parity);
}

/// Metodo para dividir un string en elementos
/// \param string String a separar
/// \param splitCharacter Caracter utilizado para separar los elementos
/// \return LinkedList con los elementos obtenidos de la separacion
LinkedList<std::string> DataBase::splitString(std::string string, char *splitCharacter) {
    LinkedList<std::string> list = LinkedList<std::string>(); //Lista en la que se guardan los elementos
    char* messageChar = strdup(string.c_str()); //Se transforma el mensaje a char*
    char* element = strtok(messageChar, splitCharacter); //Separa el char cuando lea el splitCharacter
    while (element != NULL) {
        std::string str(element);
        list.insertAtEnd(str); // Se guarda el dato en la lista
        element = strtok (NULL, splitCharacter);  // Separa el resto de la cadena cuando lea la coma
    }

    return list;
}

/// Metodo para obtener la informacion de un video que se encuentra en la tabla
/// \param name Nombre del video
/// \return LinkedList con la informacion del video, si esta no se encuentra se retorna una lista vacia
LinkedList<std::string> DataBase::getVideoData(std::string name) {
    //Se obtiene una lista con todos los videos almacenados en la tabla
    LinkedList<std::string> rows = splitString(getTable(), "/");

    //Se busca la informacion del video solicitado utilizando su nombre
    for (int i = 0; i < rows.getSize(); ++i) {
        std::string row = rows.getElement(i)->getData();
        LinkedList<std::string> data = splitString(row, ";");

        //Se compara el nombre del video actual con el nombre del video que se esta buscando
        if(strcmp(data.getElement(0)->getData().c_str(), name.c_str()) == 0){
            return data;
        }
    }

    return LinkedList<std::string>();
}

/// Metodo para obtener una de las partes de la base de datos, con el objetivo de restaurar la informacion de un disco
/// \param disk Numero de disco que se esta restaurando
/// \return String con la parte que le corresponde almacenar al disco en cuestion
std::string DataBase::getPart(int disk) {
    std::string part1 = "";
    std::string part2 = "";
    std::string part3 = "";
    std::string parity = "";

    //Se solicita cada una de las partes de la tabla a los otros discos
    for (int i = 0; i < 4; ++i) {
        if (i != disk) {
            sf::Packet packet;
            packet << "database";
            packet << "get";

            sf::TcpSocket *disk = Singleton::getDisks()->getElement(i)->getData();
            disk->send(packet);

            sf::Packet receivePacket;
            std::string receiveMessage;
            if (disk->receive(receivePacket) == sf::Socket::Done)
                receivePacket >> receiveMessage;

            if(i == 0){
                part1 = receiveMessage;
            } else if (i == 1) {
                part2 = receiveMessage;
            } else if (i == 2) {
                part3 = receiveMessage;
            } else if (i == 3) {
                parity = receiveMessage;
            }
        }
    }

    //Segun el disco que se perdio se reconstruye la informacion
    switch (disk){
        case 0:
            part1 = Tolerancia::recoverData(part2, part3, parity);
            return part1;
        case 1:
            part2 = Tolerancia::recoverData(part1, part3, parity);
            return part2;
        case 2:
            part3 = Tolerancia::recoverData(part1, part2, parity);
            return part3;
        case 3:
            std::vector<byte> vector1(part1.begin(), part1.end());
            std::vector<byte> vector2(part2.begin(), part2.end());
            std::vector<byte> vector3(part3.begin(), part3.end());
            std::vector<byte> parityVector = Tolerancia::calculateParity(vector1, vector2, vector3);
            std::string parityString(parityVector.begin(), parityVector.end());
            return parityString;
    }
}
