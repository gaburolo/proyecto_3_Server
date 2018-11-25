//
// Created by oldboy on 26/11/18.
//


#ifndef TECMFS_CONTROLLER_H
#define TECMFS_CONTROLLER_H

#include <iostream>
#include <vector>
#include <fstream>
#include "Listas/LinkedList.h"
#include "Singleton.h"
#include "Video.h"
#include "Tolerancia.h"
#include "Data.h"

typedef unsigned char byte;

class Controller {
public:
    void sendParts(std::vector<byte> data, std::string filename);

    std::string getVideo(std::string name);

    void addDisk(sf::TcpSocket *socket);

private:
    int parityDisc = 0;

    int getParityDisk();

    std::string getPart(std::string name, int diskNum);

    void restoreDisk(int diskNum);

};


#endif //TECMFS_CONTROLLER_H
