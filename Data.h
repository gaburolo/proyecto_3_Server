//
// Created by oldboy on 26/11/18.
//

#ifndef TECMFS_DATABASE_H
#define TECMFS_DATABASE_H


#include <iostream>
#include <fstream>
#include <SFML/Network/Packet.hpp>
#include "Singleton.h"
#include "Tolerancia.h"

class DataBase {
public:
    static void createTable();

    static void addToTable(std::string content);

    static std::string getTable();

    static LinkedList<std::string> splitString(std::string string, char *splitCharacter);

    static LinkedList<std::string> getVideoData(std::string name);

    static std::string getPart(int disk);

private:
    static void send(std::string content, std::string action);
};


#endif //TECMFS_DATABASE_H
