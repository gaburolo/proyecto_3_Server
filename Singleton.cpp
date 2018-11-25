//
// Created by oldboy on 26/11/18.
//


#include "Singleton.h"

Singleton* Singleton::instance = nullptr;
LinkedList<sf::TcpSocket*>* Singleton::clients = nullptr;
LinkedList<sf::TcpSocket*>* Singleton::disks = nullptr;

Singleton::Singleton() {

}

/// Metodo para instanciar el unico objeto de la clase Singleton
/// \return Un puntero a la instancia
Singleton *Singleton::getInstance() {
    //Se crean las instancias si estas no existian previamente
    if(instance == nullptr){
        instance = new Singleton();
        clients = new LinkedList<sf::TcpSocket*>();
        disks = new LinkedList<sf::TcpSocket*>();
    }

    return instance;
}

LinkedList<sf::TcpSocket*> *Singleton::getClients() {
    if(instance == nullptr){
        getInstance();
    }

    return clients;
}

LinkedList<sf::TcpSocket*> *Singleton::getDisks() {
    if(instance == nullptr){
        getInstance();
    }

    return disks;
}