//
// Created by arturocv on 13/11/17.
//

#ifndef TECMFS_NODE_H
#define TECMFS_NODE_H


template <class T>
class Node {
private:
    T data;
    Node<T> *next;

public:
    Node(T data) {
        Node::data = data;
        next = nullptr;
    }

    void setData(T data) {
        Node::data = data;
    }

    T getData() {
        return data;
    }

    void setNext(Node *next) {
        Node::next = next;
    }

    Node* getNext() {
        return next;
    }
};

#endif //TECMFS_NODE_H
