//
// Created by oldboy on 26/11/18.
//

#ifndef TECMFS_VIDEOHANDLER_H
#define TECMFS_VIDEOHANDLER_H

#include <iostream>
#include <SFML/Network.hpp>
#include <cstring>
#include <fstream>
#include "Singleton.h"

typedef unsigned char byte;

class VideoHandler {
public:
    static std::vector<byte> receiveVideo(sf::TcpSocket *client);

    static LinkedList<std::vector<byte>> splitVideo(std::vector<byte> vector);

    static std::string joinVideo(std::string part1, std::string part2, std::string part3);
};


#endif //TECMFS_VIDEOHANDLER_H
