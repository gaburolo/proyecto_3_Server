//
// Created by oldboy on 26/11/18.
//

#ifndef TECMFS_FAULTTOLERANCE_H
#define TECMFS_FAULTTOLERANCE_H


#include <iostream>
#include <fstream>
#include <vector>

typedef unsigned char byte;

class Tolerancia {
public:
    /// Metodo que utiliza el XOR para calcular la paridad de un archivo
    /// \param part1 Parte uno del archivo
    /// \param part2 Parte dos del archivo
    /// \param part3 Parte tres del archivo
    /// \return Vector con la paridad
    static std::vector<byte> calculateParity(std::vector<byte> part1, std::vector<byte> part2, std::vector<byte> part3) {
        std::vector<byte> vectorParity;

        //Por cada byte de las partes se aplica el XOR y se añade el resultado al vector de paridad
        for(int i = 0; i < part3.size(); i++) {
            vectorParity.push_back(part1[i] ^ part2[i] ^ part3[i]);
        }

        return vectorParity;
    }

    /// Metodo para recuperar una parte del archivo utilizando la paridad del mismo
    /// \param partAString Parte del archivo
    /// \param partBString Parte del archivo
    /// \param parityString Paridad del archivo
    /// \return Vector con la parte del archivo que se perdio
    static std::string recoverData(std::string partAString, std::string partBString, std::string parityString) {
        std::vector<byte> vectorFault;

        //Se transforman los strings a vectores
        std::vector<byte> partA(partAString.begin(), partAString.end());
        std::vector<byte> partB(partBString.begin(), partBString.end());
        std::vector<byte> parity(parityString.begin(), parityString.end());

        //Por cada byte del vector se utiliza el XOR para obtener la parte perdida
        for(int i = 0; i < parity.size(); i++) {
            vectorFault.push_back(partA[i] ^ partB[i] ^ parity[i]);
        }

        if(partA.size() != partB.size()) {
            for (int j = 0; j < (parity.size() - partA.size()); ++j) {
                vectorFault.erase(vectorFault.begin() + (vectorFault.size() - 1));
            }
        }

        //Se transforma el vector obtenido en un string
        std::string reconstructedPart(vectorFault.begin(), vectorFault.end());
        return reconstructedPart;
    }
};


#endif //TECMFS_FAULTTOLERANCE_H
