//
// Created by oldboy on 26/11/18.
//

#include <SFML/Network.hpp>
#include <iostream>
#include "Video.h"


#define PORT 2001

using namespace std;

typedef unsigned char byte;

int main(int argc, char const *argv[]) {
    std::cout << "IP: " << sf::IpAddress::getLocalAddress() << std::endl;

    Controller controller = Controller();
    sf::TcpListener listener;
    sf::SocketSelector selector;

    LinkedList <sf::TcpSocket*>* clients = Singleton::getClients();
    LinkedList<sf::TcpSocket*>* disks = Singleton::getDisks();

    listener.listen(PORT);
    selector.add(listener);

    while(true) {
        if(selector.wait()) {

            //Se aceptan nuevos clientes
            if(selector.isReady(listener)) {
                sf::TcpSocket *socket = new sf::TcpSocket;
                listener.accept(*socket);
                sf::Packet packet;
                string id;
                if (socket->receive(packet) == sf::Socket::Done)
                    packet >> id;
                if (id == "Disk") {

                    controller.addDisk(socket);
                    selector.add(*socket);

                } else {
                    cout << "Se ha conectado un nuevo cliente!" << endl;
                    clients->insertAtEnd(socket);
                    selector.add(*socket);
                }

            //Se verifica si algun cliente hizo una solicitud
            } else if(disks->getSize() == 4){
                std::cout<<disks->getSize()<<std::endl;
                ///Clients
                for(int i=0; i<clients->getSize(); i++) {
                    if(selector.isReady(*clients->getElement(i)->getData())) {
                        sf::Packet receivePacket, sendPacket;
                        if(clients->getElement(i)->getData()->receive(receivePacket) == sf::Socket::Done) {
                            ///Clients Actions
                            std::string stringAction;
                            receivePacket >> stringAction;
                            char* action = (char*) stringAction.c_str();

                            //Se realiza una solicitud de almacenaje
                            if(strcmp(action, "saveVideo") == 0){
                                std::string fileName;
                                receivePacket >> fileName;
                                std::vector<byte> video = VideoHandler::receiveVideo(clients->getElement(i)->getData());
                                controller.sendParts(video, fileName);

                            //Se realiza una solicitud de obtencion de un video
                            } else if(strcmp(action, "getVideo") == 0){
                                std::string fileName;
                                receivePacket >> fileName;
                                std::string video = controller.getVideo(fileName);
                                sf::Packet responsePacket;
                                responsePacket << video;
                                responsePacket << DataBase::getVideoData(fileName).getElement(1)->getData();
                                clients->getElement(i)->getData()->send(responsePacket);

                            //Se realiza una solicitud de obtencion de la tabla de la base de datos
                            } else if(strcmp(action, "getTable") == 0){
                                std::string table = DataBase::getTable();
                                sf::Packet responsePacket;
                                responsePacket << table;
                                clients->getElement(i)->getData()->send(responsePacket);
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}
