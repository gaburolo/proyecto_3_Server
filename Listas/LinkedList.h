//
// Created by arturocv on 13/11/17.
//

#ifndef TECMFS_LINKEDLIST_H
#define TECMFS_LINKEDLIST_H


#include <iostream>
#include "Node.h"

template <class T>
class LinkedList {
private:
    Node<T> *first;
    int size;

public:
    /// Constructor
    /// \tparam T Tipo de dato de la lista
    LinkedList(){
        first = nullptr;
        size = 0;
    }

    int getSize() {
        return size;
    }

    /// Metodo para insertar un nuevo elemento al inicio de la lista.
    /// \tparam T Tipo del dato que se desea insertar.
    /// \param data Dato que se desea insertar.
    void insertAtFirst(T data) {
        Node<T> *newNode = new Node<T>(data);
        if(first == nullptr){
            first = newNode;
        } else {
            newNode->setNext(first);
            first = newNode;
        }
        size++;
    }

    /// Metodo que inserta un nuevo elemento al final de la lista
    /// \tparam T Tipo de dato del elemento
    /// \param data Dato del nuevo elemento
    void insertAtEnd(T data) {
        Node<T> *newNode = new Node<T>(data);
        if (first == nullptr) {
            first = newNode;
        } else {
            Node<T> *current = first;
            while (current->getNext() != nullptr) {
                current = current->getNext();
            }
            current->setNext(newNode);
        }
        size++;
    }

    /// Metodo que inserta un nuevo elemento en una posicion especifica
    /// \tparam T Tipo de dato del nuevo elemento
    /// \param data Dato del nuevo elemento
    /// \param position Posicion para el nuevo elemento
    void insertElement(T data, int position) {
        Node<T> *newNode = new Node<T>(data);
        Node<T> *current = first;

        if(first != nullptr) {
            if (1 <= position && position < size) {
                for (int i = 0; i < (position - 1); ++i) { //Se detiene en el nodo anterior al de la posición deseada
                    current = current->getNext();
                }
                newNode->setNext(current->getNext()); //Al nuevo nodo se le asigna la referencia al nodo siguiente
                current->setNext(newNode); //Al nodo anterior al nuevo nodo se le asigna la referencia al nuevo nodo
            }
            if (position == 0) { //caso en que la posicion deseada es como primer elemento
                newNode->setNext(first);
                first = newNode;
            }
            if (size <= position) {
                for (int i = 0; i < (size - 1); ++i) { //Se detiene en el último nodo de la lista
                    current = current->getNext();
                }
                current->setNext(newNode);
            }
        } else{
            first = newNode;
        }
        size++;
    }


    /// Metodo para obtener el elemento de una posicion especifica
    /// \tparam T Tipo de dato del elemento deseado
    /// \param position Posicion del elemento deseado
    /// \return El elemento deseado
    Node<T>* getElement(int position) {
        Node<T> *current = first;

        if(position < size && position >= 0) {
            for (int i = 0; i < position; i++) {
                current = current->getNext();
            }
        } else{
            return nullptr;
        }
        return current;
    }

    Node<T>* getElement2(T data) {
        Node<T> *current = first; //Punto de partida en la busqueda

        for (int i = 0; i < size; ++i) {
            if (current->getData() == data){ //Coincide el elemento actual con el que se esta buscando
                return current; //Se retorna
            } else //No coinciden los elementos
                current = current->getNext(); //Se continua con el siguiente nodo en la lista
        }
        Node<T>* notFoundNode = new Node<T>(nullptr); //Se crea un nodo con un data nulo
        return notFoundNode; //Elemento no estaba en la lista
    }


    /// Metodo para establecer el dato que se estaba guardando en una posicion especifica
    /// \tparam T Tipo del dato que se desea guardar
    /// \param element Elemento que se desea almacenar
    /// \param position Posicion en la que se desea almacenar el elemento
    void setElement(T element, int position) {
        Node<T> *current = first;

        if (first != nullptr && position < size) {
            if (1 <= position && position < size) {
                for (int i = 0; i < (position - 1); i++) { //Se detiene en el nodo anterior al de la posición deseada
                    current = current->getNext();
                }
                current->getNext()->setData(element);
            }
            if (position == 0) { //caso en que la posicion deseada es como primer elemento
                first->setData(element);
            }
            if (size <= position) {
                for (int i = 0; i < (size - 1); i++) { //Se detiene en el último nodo de la lista
                    current = current->getNext();
                }
                current->getNext()->setData(element);
            }
        }
    }

    /// Metodo para eliminar un elemento en una posicion especifica
    /// \tparam T Tipo de dato del los elementos de la lista
    /// \param position Posicion del elemento a eliminar
    void deleteElement(int position) {
        if (position == 0 && size > 0) {
            first = first->getNext();
            size--;
        } else {
            if (position <= (size - 1)) {
                Node<T>* current = first;
                for (int i = 0; i < (position - 1); i++) {
                    current = current->getNext();
                }
                current->setNext(current->getNext()->getNext());
                size--;
            }
        }
    }

    /// Metodo para eliminar un elemento de la lista
    /// \tparam T Tipo de dato de la lista
    /// \param data Elemento que se desea eliminar
    void deleteElement2(T data) {
        Node<T> *current = first;

        if (size > 0){
            if (*current->getData() == *data) {
                first = first->getNext();
                this->size--;
            } else {
                while (current != nullptr && current->getNext() != nullptr) {
                    if (*current->getNext()->getData() == *data) {
                        current->setNext(current->getNext()->getNext());
                        this->size--;
                    }
                    current = current->getNext();
                }
            }
        }
    }

    /// Metodo que verifica si dentro de la lista se encuentra un elemento ingresado
    /// \tparam T Tipo de dato de la lista
    /// \param data Elemento que se esta buscando
    /// \return True si el elemento esta dentro de la lista, false si no lo esta
    bool containsElement(T data) {
        Node<T> *current = first; //Punto de partida en la busqueda

        for (int i = 0; i < size; ++i) {
            if (current->getData() == data){ //Coincide el elemento actual con el que se esta buscando
                return true; //Se retorna true
            } else //No coinciden los elementos
                current = current->getNext(); //Se continua con el siguiente nodo en la lista
        }
        return false; //No se ha encontrado el elemento en la lista
    }

    void printList() {
        Node<T> *current = first;
        while(current != nullptr){
            std::cout << (current->getData()) << ", ";
            current = current->getNext();
        }
        std::cout << std::endl;
    }
};

#endif //TECMFS_LINKEDLIST_H
