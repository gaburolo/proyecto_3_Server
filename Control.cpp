
//
// Created by oldboy on 26/11/18.
//
#include "Control.h"


/// Metodo para dividir el video y enviar las partes a los discos
/// \param data Vector con el video
/// \param filename Nombre del archivo del video
void Controller::sendParts(std::vector<byte> data, std::string filename) {
    //Se obtienen las partes del video y la paridad
    LinkedList<std::vector<byte>> parts = VideoHandler::splitVideo(data);
    std::vector<byte> parity = Tolerancia::calculateParity(parts.getElement(0)->getData(), parts.getElement(1)->getData(), parts.getElement(2)->getData());

    //Se obtiene el nombre y la extension del fichero
    LinkedList<std::string> list = DataBase::splitString(filename, ".");
    std::string name = list.getElement(0)->getData();
    std::string extension = list.getElement(1)->getData();

    //Se forma el string que almacena el nuevo elemento de la base de datos
    std::string newElement = name + ";" + extension + ";" + std::to_string(data.size()) + ";";
    std::string diskParity;

    int parityDisk = getParityDisk();
    int currentPart = 0;

    //Se envian las partes a cada uno de los discos
    for (int i = 0; i < 4; ++i) {
        sf::Packet packet;
        packet << "savePart";

        //El disco actual es al que le corresponde la paridad
        if(i == parityDisk){
            packet << name + "_parity";
            packet << std::string(parity.begin(), parity.end());
            diskParity = std::to_string(i);

        //Al disco actual no le corresponde la paridad
        } else {
            packet << name + std::to_string(currentPart + 1);
            std::vector<byte> part = parts.getElement(currentPart)->getData();
            packet << std::string(part.begin(), part.end());
            newElement.append(std::to_string(i) + ";");
            currentPart++;

        }

        //Se envia el packet
        Singleton::getDisks()->getElement(i)->getData()->send(packet);
    }

    //Se termina de formar el nuevo elemento y se envia a la base de datos
    newElement.append(diskParity);
    DataBase::addToTable(newElement);

}

/// Metodo para obtener a cual disco le corresponde la paridad
/// \return Posicion del disco que le corresponde la paridad
int Controller::getParityDisk() {
    int disc = parityDisc;
    parityDisc++;

    if(parityDisc > 3){
        parityDisc = 0;
    }

    return disc;
}

/// Metodo para obtener las partes del video almacenadas en los discos
/// \param name Nombre del video
/// \return Un string del video
std::string Controller::getVideo(std::string name) {
    LinkedList<std::string> data = DataBase::getVideoData(name);

    //Se verifica que se obtuvo el video de manera correcta
    if(data.getSize() > 0){
        std::string part1 = "";
        std::string part2 = "";
        std::string part3 = "";
        std::string parity = "";

        int currentPart = 0;

        //Se solicitan las partes del video a cada uno de los discos
        for (int i = 1; i < 5; ++i) {
            //Se obtiene el disco en el que se encuentra la parte actual
            int diskNum = std::stoi(data.getElement(2+i)->getData());

            //Se obtiene la parte actual del video
            if(i == 1){
                part1 = getPart(name + std::to_string(currentPart + 1), diskNum);
            } else if(i == 2){
                part2 = getPart(name + std::to_string(currentPart + 1), diskNum);
            } else if(i == 3){
                part3 = getPart(name + std::to_string(currentPart + 1), diskNum);
            } else if(i == 4){
                parity = getPart(name + "_parity", diskNum);
            }
            currentPart++;
        }

        //Casos en los que uno de los discos tuvo un fallo
        if(part1.size() == 0){
            part1 = Tolerancia::recoverData(part2, part3, parity);
        } else if(part2.size() == 0){
            part2 = Tolerancia::recoverData(part1, part3, parity);
        } else if(part3.size() == 0){
            part3 = Tolerancia::recoverData(part1, part2, parity);
        }

        //Se reconstruye el video
        std::string video = VideoHandler::joinVideo(part1, part2, part3);

        return video;
    }

}

/// Metodo para obtener una parte del video de un disco en especifico
/// \param name Nombre del video
/// \param diskNum Numero del disco
/// \return Un string con la parte obtenida del disco
std::string Controller::getPart(std::string name, int diskNum) {
    //Se obtiene el disco solicitado
    sf::TcpSocket* disk = Singleton::getDisks()->getElement(diskNum)->getData();

    //Se le indica al disco que se esta solicitando una parte de un video
    sf::Packet packet;
    packet << "getPart";
    packet << name;
    disk->send(packet);

    //Se espera la respuesta del disco
    sf::Packet receivePacket;
    std::string receiveMessage = "";

    if (disk->receive(receivePacket) == sf::Socket::Done) {
        receivePacket >> receiveMessage;
    }

    return receiveMessage;
}

/// Metodo para agregar un disco a la lista
/// \param socket Disco que se desea agregar
void Controller::addDisk(sf::TcpSocket *socket) {
    int i = 0;
    while(i != 4) {
        //Se verifica si ya se tiene la cantidad maxima de discos
        if (Singleton::getDisks()->getSize() < 4) {
            Singleton::getDisks()->insertAtEnd(socket);
            std::cout << "Se ha identificado un disco nuevo" << std::endl;

            //Si ya se conectaron los cuatro discos se procede a crear la tabla
            if(Singleton::getDisks()->getSize() == 4)
                DataBase::createTable();

            break;
        } else {
            //Se verifica si algun disco se encuentra desconectado
            sf::TcpSocket *disk = Singleton::getDisks()->getElement(i)->getData();
            sf::Packet packet;
            packet << "verifyingConnection...";
            disk->send(packet);
            bool connected = false;
            if (disk->receive(packet) == sf::Socket::Done)
                connected = true;

            if(!connected){
                Singleton::getDisks()->setElement(socket, i);
                //restoreDisk(i);
                //std::cout << "Disco " << (i + 1) << " restaurado" << std::endl;
                break;
            }
        }
        i++;
    }
}

/// Metodo para restaurar la informacion que se encontraba en un disco
/// \param diskNum Posicion en la lista de discos del disco que hay que restaurar
void Controller::restoreDisk(int diskNum) {
    sf::TcpSocket *disk = Singleton::getDisks()->getElement(diskNum)->getData();

    //Se reconstruye la informacion de la base de datos y se le envia al disco
    //la seccion correspondiente
    std::string dataBase = DataBase::getPart(diskNum);
    sf::Packet dataPacket;
    dataPacket << "database";
    dataPacket << "save";
    dataPacket << dataBase;
    disk->send(dataPacket);

    //Se reconstruye la parte perdida del video y se le envia al disco
    LinkedList<std::string> elements = DataBase::splitString(DataBase::getTable(), "/");

    for (int j = 1; j < elements.getSize(); ++j) {
        LinkedList<std::string> data = DataBase::splitString(elements.getElement(j)->getData(), ";");
        if (data.getSize() > 0) {
            std::string name = data.getElement(0)->getData();

            sf::Packet partPacket;
            partPacket << "savePart";

            std::string part1 = "";
            std::string part2 = "";
            std::string part3 = "";
            std::string parity = "";

            int currentPart = 0;

            //Se solicitan las partes del video a cada uno de los discos
            for (int k = 1; k < 5; ++k) {
                //Se obtiene el disco en el que se encuentra la parte actual
                int numDisk = std::stoi(data.getElement(2 + k)->getData());
                if(diskNum != numDisk) {
                    //Se obtiene la parte actual del video
                    if (k == 1) {
                        part1 = getPart(name + std::to_string(currentPart + 1), numDisk);
                    } else if (k == 2) {
                        part2 = getPart(name + std::to_string(currentPart + 1), numDisk);
                    } else if (k == 3) {
                        part3 = getPart(name + std::to_string(currentPart + 1), numDisk);
                    } else if (k == 4) {
                        parity = getPart(name + "_parity", numDisk);
                    }
                }
                currentPart++;
            }

            //Se reconstruye la parte del disco que tuvo un fallo
            if (part1.size() == 0) {
                partPacket << name + "1";
                part1 = Tolerancia::recoverData(part2, part3, parity);
                partPacket << part1;

            } else if (part2.size() == 0) {
                partPacket << name + "2";
                part2 = Tolerancia::recoverData(part1, part3, parity);
                partPacket << part2;

            } else if (part3.size() == 0) {
                partPacket << name + "3";
                part3 = Tolerancia::recoverData(part1, part2, parity);
                partPacket << part3;

            } else if (parity.size() == 0) {
                partPacket << name + "_parity";
                std::vector<byte> vector1(part1.begin(), part1.end());
                std::vector<byte> vector2(part2.begin(), part2.end());
                std::vector<byte> vector3(part3.begin(), part3.end());
                std::vector<byte> parityVector = Tolerancia::calculateParity(vector1, vector2, vector3);
                parity = std::string(parityVector.begin(), parityVector.end());
                partPacket << parity;
            }

            disk->send(partPacket);
        }
    }
}
