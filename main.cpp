#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

#define PORT 2000

using namespace std;

int main(int argc, char const *argv[])
{
    ///IP Address: 10.0.2.15
    sf::IpAddress IP = sf::IpAddress::getLocalAddress();
    cout << IP << "\n" << endl;

    sf::TcpListener listener;
    sf::SocketSelector selector;
    vector<sf::TcpSocket*> clients;

    listener.listen(PORT);
    selector.add(listener);

    bool done = false;
    while(!done)
    {
        ///Clients
        if(selector.wait())
        {
            if(selector.isReady(listener))
            {
                sf::TcpSocket *socket = new sf::TcpSocket;
                listener.accept(*socket);
                sf::Packet packet;
                string id;
                if(socket->receive(packet) == sf::Socket::Done)
                    packet>>id;

                cout << id << " se ha conectado al TECMFS\n" << endl;
                clients.push_back(socket);
                selector.add(*socket);
            }//if(selector.isReady(listener))
            else
            {
                for(int i=0; i<clients.size(); i++)
                {
                    if(selector.isReady(*clients[i]))
                    {
                        sf::Packet receivePacket, sendPacket;
                        if(clients[i]->receive(receivePacket) == sf::Socket::Done)
                        {
                            ///Actions
                        }
                    }
                }
            }
        }

        //for(vector<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); it++)
        //    delete *it;

    }//while(!done)
}