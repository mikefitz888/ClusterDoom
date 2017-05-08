#include "../include/network/Network.h"
#include "../include/manager.h"
#include "../include/gamecontroller.h"
#include "../include/gameobject.h"
#include "../include/GameObjects/PlayerInstance.h"

namespace network {
    
    //// -------------------------------------------------------------------------------- //////
    // NETWORK MANAGER
    NetworkManager::NetworkManager(Manager *manager) {
        this->manager = manager;
        listener = new sf::TcpListener();
        port = 31655;

        sf::Socket::Status st = listener->listen(port);
        if (st != sf::Socket::Done) {
            std::cout << "NETWORK: Failed to initialise server on port: " << port << std::endl;
            release();
        }
        else {

            // Success!
            listener->setBlocking(false);
            std::cout << "[SERVER] Server started on port " << port << "! Ready to accept new connctions!" << std::endl;

            // Prepare new client socket
            new_client = new sf::TcpSocket();
        }
    }

    void NetworkManager::networkStep() {

        // Listen for new connections
        sf::Socket::Status st = listener->accept(*new_client);
        if (st == sf::Socket::Done) {
            sf::IpAddress ip = new_client->getRemoteAddress();
            std::cout << "New connection received from " << ip << ":" << new_client->getRemotePort() << std::endl;

            // Handle new connection:
            newConnection(new_client, ip);

            // Prepare new client socket
            new_client = new sf::TcpSocket();

        }
        else if (st == sf::Socket::Error || st == sf::Socket::Disconnected) {
            std::cout << "An error occured: " << st << std::endl;
            release();
        }

        // Process existing connections
        for (NetworkClient* it : clients) {
            it->processConnection(manager);
        }

    }

    void NetworkManager::newConnection(sf::TcpSocket *sfc, sf::IpAddress ip) {
        NetworkClient *new_client = new NetworkClient(sfc, ip, this); //NetworkClient(sf::TcpSocket *socket, sf::IpAddress ip, NetworkManager* manager);
        clients.push_back(new_client);
        new_client->setID(connection_identifier_max);
        connection_identifier_max++;
        // Begin security process on new client
        new_client->beginSecurityProcess();

        std::cout << "\t\tTOTAL CONNECTIONS: " << clients.size() << std::endl << std::endl;
    }

    void NetworkManager::removeConnection(NetworkClient *network_client) {
        auto nc = std::find(clients.begin(), clients.end(), network_client);
        if (nc != clients.end()) {
            clients.erase(nc);
            std::cout << "[SERVER] Connection Removed! Total connections: " << clients.size() << std::endl;
        }
    }

    Buffer& NetworkManager::getSendBuffer() {
        return this->send_buffer;
    }

    /*
        Sends the given buffer to all clients
    */
    void NetworkManager::sendToAll(Buffer &buff) {
        for (NetworkClient* it : clients) {
            if (it->getConnectionStatus() == NetworkClient::VERIFIED) {
                it->sendPacket(buff);
            }
        }
    }

    // Invokes an instance creation on all clients
    void NetworkManager::sendInstanceCreate(int instance_id, int instance_super_type, int instance_sub_type, int x, int y) {
        for (NetworkClient* it : clients) {
            it->sendInstanceCreate(instance_id, instance_super_type, instance_sub_type, x, y);
        }
    }

    void NetworkManager::sendInstanceCreate(NetworkClient *client, int instance_id, int instance_super_type, int instance_sub_type, int x, int y) {
        client->sendInstanceCreate(instance_id, instance_super_type, instance_sub_type, x, y);
    }

    // Invokes an instance destroy on all clients
    void NetworkManager::sendInstanceDestroy(int instance_id) {
        for (NetworkClient* it : clients) {
            it->sendInstanceDestroy(instance_id);
        }
    }

    // Send all existing instances to the new client:
    void NetworkManager::sendAllInstancesToClient(NetworkClient *network_client) {
        manager->sendAllInstancesToClient(network_client);
    }



    void NetworkManager::release() {
        for (NetworkClient* it : clients) {
            it->release();
            delete it;
        }
        listener->close();
    }

    int NetworkManager::getNumConnections() {
        return this->clients.size();
    }

    bool NetworkManager::getPlayerExists(int player_id) {
        for (auto& c : this->clients) {
            if (c->connection_id == player_id) {
                return true;
            }
        }
        return false;
    }


    //// -------------------------------------------------------------------------------- //////
    // NETWORK CLIENT
    NetworkClient::NetworkClient(sf::TcpSocket *socket, sf::IpAddress ip, NetworkManager* manager) {
        this->socket = socket;
        this->ip = ip;
        this->network_manager = manager;
        this->socket->setBlocking(false);
    }

    void NetworkClient::setID(int connection_id) {
        this->connection_id = connection_id;
    }

    /*
    Initialises the security process for the client.
    This is a simple evaluation process to prevent
    clients from un-authorised sources connecting.

    The process works by sending a random security
    key to a client, performing a hash on that
    security key and awaiting for the return of
    the identical hash'd result from the client.
    */
    void NetworkClient::beginSecurityProcess() {
        // Mark as being in security process
        connection_state = UNVERIFIED;
        // Generate random number
        std::srand(((unsigned int) std::time(0)) * 1458484);
        security_token = rand();

        // Write a packet
        send_buffer.seek(4);
        send_buffer << NetworkManager::SERVER_PACKET_TYPE::SendWelcome;
        send_buffer << security_token;

        // Write size
        unsigned int size = send_buffer.tell();
        send_buffer.seek(0);
        send_buffer << (unsigned int)size;
        send_buffer.seek(size); // < Jump back to previous end

        socket->setBlocking(true);
        socket->send(send_buffer.getPtr(), size);
        socket->setBlocking(false);

        // Debug
        std::cout << "[ClientSecurityProcess] Sent security token: " << security_token << " to client" << std::endl;

        // Prepare security hash
        prepareSecurityHash();
    }

    void NetworkClient::prepareSecurityHash() {
        security_hash = security_token;
        security_hash *= 22;
        security_hash += 1337;
        security_hash >>= 1;
        security_hash &= 41256235;
        security_hash -= 3590324;
        connection_state = SECURITY;
    }
	
    /*
        Completes the security process by comparing the 
        received token against the stored security hash.

        If they are the same, the client has correctly
        validated itself.
    */
    void NetworkClient::completeSecurityProcess(int received_token) {
		std::cout << "[network client] recieved token: " << received_token << std::endl;
		std::cout << "[network client] security hash: " << security_hash << std::endl;
        if (received_token == security_hash) {
            connection_state = VERIFIED;
            std::cout << "[NETWORK CLIENT] Client successfully verified!" << std::endl;

            // Send verify packet
			send_buffer.seek(4);
			send_buffer << NetworkManager::SERVER_PACKET_TYPE::SendConfirmConnect;

            // Write size
            unsigned int size = send_buffer.tell();
            send_buffer.seek(0);
            send_buffer << (unsigned int)size;
            send_buffer.seek(size); // < Jump back to previous end

            socket->setBlocking(true);
			socket->send(send_buffer.getPtr(), size);
            socket->setBlocking(false);

            // Send all instances
            network_manager->sendAllInstancesToClient(this);
        } else {
            // Disconnect
            disconnect("Security Check Invalid.");
        }
    }

    /*
        Send Global game network update states:
    */
    void NetworkClient::sendInstanceCreate(int instance_id, int instance_super_type, int instance_sub_type, int x, int y) {

        gameobject_ptr inst = this->network_manager->manager->getObjectById(instance_id);
        if (inst && inst->getNetworkSend()) {
            //socket->setBlocking(true);
            size_t ss;
            std::cout << "[NETWORK CLIENT] SENDING INSTANCE: " << instance_id << " to client: " << ip << std::endl;
            send_buffer.seek(4);
            send_buffer << NetworkManager::SERVER_PACKET_TYPE::SendInstanceCreate;
            send_buffer << (unsigned int)instance_id;
            send_buffer << (unsigned int)instance_super_type;
            send_buffer << (unsigned int)instance_sub_type;
            send_buffer << (unsigned int)x;
            send_buffer << (unsigned int)y;

            // Write size
            unsigned int size = send_buffer.tell();
            send_buffer.seek(0);
            send_buffer << (unsigned int)size;
            send_buffer.seek(size); // < Jump back to previous end

            socket->setBlocking(true);
            socket->send(send_buffer.getPtr(), size, ss);
            socket->setBlocking(false);
            std::cout << "STATUS: " << ss << std::endl;
            //socket->setBlocking(false);
        }
    }

    void NetworkClient::sendInstanceDestroy(int instance_id) {
       // gameobject_ptr inst = this->network_manager->manager->getObjectById(instance_id); // << Check needs to be in manager as the object will already be gone at this point
      //  if (inst && inst->getNetworkSend()) {
            send_buffer.seek(4);
            send_buffer << NetworkManager::SERVER_PACKET_TYPE::SendInstanceDestroy;
            send_buffer << (unsigned int)instance_id;

            // Write size
            unsigned int size = send_buffer.tell();
            send_buffer.seek(0);
            send_buffer << (unsigned int)size;
            send_buffer.seek(size); // < Jump back to previous end

            socket->setBlocking(true);
            socket->send(send_buffer.getPtr(), size);
            socket->setBlocking(false);
       // }
    }

    /*
        Here, we are both listening for new, incomming data
        and also testing that the socket is still live.

        If the socket is not live, we will either receive
        an error flag, or a disconnect flag.

        We can capture this and gracefully remove the connection
        from the network manager.

        Otherwise, we can handle the data that has been received.
    */
    void NetworkClient::listenForData(Manager *manager) {

        // Check for existence of PlayerIntsance
        if (this->connection_state == ConnectionState::VERIFIED) {
            if (manager->getGameController()->getGameState() == gamecontroller::GameState::RUNNING) {
                if (this->player_instance == nullptr || !player_instance) {
                    // Create object
                    gameobject_ptr obj = manager->getGameController()->spawnObjectAt(gameobject::OBJECT_TYPE::PLAYER_INSTANCE, 0.0f, 0.0f);
                    if (obj) {
                        this->player_instance = smartpointers::static_pointer_cast<PlayerInstance>(obj);
                        this->player_instance->setNetworkInstanceID(this, this->connection_id);

                        // SEND PLAYER ID & GAMEOBJECT ID TO CLIENT
                        send_buffer.seek(4);
                        send_buffer << NetworkManager::SERVER_PACKET_TYPE::SendPlayerInstanceID;
                        send_buffer << (unsigned int)this->player_instance->getID();
                        send_buffer << (unsigned int)this->connection_id;

                        // Write size
                        unsigned int size = send_buffer.tell();
                        send_buffer.seek(0);
                        send_buffer << (unsigned int)size;
                        send_buffer.seek(size); // < Jump back to previous end

                        socket->setBlocking(true);
                        socket->send(send_buffer.getPtr(), size);
                        socket->setBlocking(false);
                    }
                
                }
            }
        }


        // Listen for data
        std::size_t message_size;
        sf::TcpSocket::Status st = socket->receive((void*)recv_buffer.getPtr(), (std::size_t)recv_buffer.maxSize(), message_size);
        switch (st) {
            case sf::TcpSocket::Done: {
                // Data received
                std::cout << "[NETWORK CLIENT] Data received from " << ip << ". Size: " << message_size << " bytes!" << std::endl;

                // Read packet:
                recv_buffer.seek(0);
                unsigned int global_event_id = 0;
                recv_buffer >> global_event_id;

                switch (global_event_id) {

                    case NetworkManager::CLIENT_PACKET_TYPE::RecvSecurityInfo: {
                        int recv_token = 0;
                        recv_buffer >> recv_token;

                        completeSecurityProcess(recv_token);
                    } break;

                    case NetworkManager::CLIENT_PACKET_TYPE::RecvDisconnect:
                        std::cout << "[NETWORK CLIENT] Client " << ip << " has requested disconnection. Disconnecting!" << std::endl;
                        disconnect("Player disconnected from the server");
                        break;

                    case NetworkManager::CLIENT_PACKET_TYPE::RecvInstanceInteraction:{
                        // TODO: Respond to instance interactions from clients
                        std::cout << "[NETWORK CLIENT] Client " << ip << " has sent a instance interaction!" << std::endl;
                        /*int x;
                        int y;
                        recv_buffer >> x;
                        recv_buffer >> y;
                        manager->getGameController()->spawnUnitAt(gameobject::Point<int>(x, y), (unit::TYPE) 1);*/

                        unsigned int object_id = 0;
                        unsigned int event_id = 0;

                        recv_buffer >> object_id;
                        recv_buffer >> event_id;

                        // Check if object exists
                        gameobject_ptr obj = manager->getObjectById(object_id);
                        if (obj) {
                            obj->recvNetworkInteraction(event_id, recv_buffer, this);
                        }
                    } break;

                    default: // INVALID PACKET RECEIVED FROM CLIENT
                        std::cout << "[NETWORK CLIENT] ERROR: Invalid packet received from " << ip << ". Disconnecting! Event: " << global_event_id << std::endl;
                        disconnect("Invalid Request Rejected By Server");
                        break;
                }
            } break;
            case sf::TcpSocket::Error:
            case sf::TcpSocket::Disconnected:
                release();
                break;
        }
    }

    void NetworkClient::processConnection(Manager *manager) {
        // Send ping
        ping_packet_timer--;
        if (ping_packet_timer <= 0) {
            ping_packet_timer = ping_packet_timer_max;

            // Send ping packet
            send_buffer.seek(4);
            send_buffer << NetworkManager::SERVER_PACKET_TYPE::SendPing;

            // Write size
            unsigned int size = send_buffer.tell();
            send_buffer.seek(0);
            send_buffer << (unsigned int)size;
            send_buffer.seek(size); // < Jump back to previous end

            // Send
            socket->setBlocking(true);
            socket->send(send_buffer.getPtr(), size);
            socket->setBlocking(false);
        }

        // Listen for incoming data
        listenForData(manager);
    }

    void NetworkClient::disconnect(char* reason) {
        if (connection_state != DISCONNECTED) {
            // Send packet to gracefully disconnect
            send_buffer.seek(4);
            send_buffer << NetworkManager::SERVER_PACKET_TYPE::SendDisconnect;
            send_buffer << "You have been disconnected by the server";    // <- Message
            send_buffer << reason;    // <- Reason

            // Write size
            unsigned int size = send_buffer.tell();
            send_buffer.seek(0);
            send_buffer << (unsigned int)size;
            send_buffer.seek(size); // < Jump back to previous end

            socket->setBlocking(true);
            socket->send(send_buffer.getPtr(), size);
            socket->setBlocking(false);

            std::cout << "[NETWORK CLIENT] Client " << ip << " has been disconnected." << std::endl << "\t\tREASON: " << reason << std::endl;
            connection_state = DISCONNECTED;
            release(); // We have to call release. This may result in a double call, however it guarantees that everything gets cleaned up, no matter which function was called.
        }
    }

    void NetworkClient::sendPacket(Buffer &buff) const {
        
        int size = buff.tell();
        if (size > 0) {
            socket->setBlocking(true);
            socket->send(buff.getPtr(), size);
            socket->setBlocking(false);
        } else {
            std::cout << "[NETWORK CLIENT] ERROR: Attempting to send packet of size 0." << std::endl;
        }
    }

    NetworkClient::ConnectionState NetworkClient::getConnectionStatus() const {
        return connection_state;
    }

    void NetworkClient::release() {

        // Send disconnect message
        disconnect((char*) "Connection Closed By Server");

        // Disconnect socket
        socket->disconnect();

        // Remove connection from the Networking main array
        network_manager->removeConnection(this);
    }

    smartpointers::slave_ptr<PlayerInstance> NetworkClient::getPlayerInstance() {
        return this->player_instance;
    }

    //// -------------------------------------------------------------------------------- //////
    // INetworkInstance (interface)
    void INetworkInstance::sendNetworkUpdate(int event_id){
        if (this->getInstance()->getNetworkSend()) {
            Buffer& buff = manager->getSendBuffer();
            buff.seek(4);
            buff << NetworkManager::SERVER_PACKET_TYPE::SendInstanceUpdate;
            buff << (unsigned int)network_instance_id;
            buff << (unsigned int)event_id;

            writeNetworkUpdate(event_id, buff); // <- User interface function to write data

            /*std::cout << "[NETWORK] Sending network object update: " << std::endl <<
                "     Size: " << buff.tell() << std::endl <<
                "     Instance ID: " << network_instance_id << std::endl <<
                "     Event ID:    " << event_id << std::endl;*/

                // Write size
            unsigned int size = buff.tell();
            buff.seek(0);
            buff << (unsigned int)size;
            buff.seek(size); // < Jump back to previous end

            // Broadcast update to all clients
            manager->sendToAll(buff);
        }
    }

	// Set manager
	void INetworkInstance::setNetworkManager(NetworkManager* network_manager) {
		this->manager = network_manager;
	}
	void INetworkInstance::setNetworkID(gameobject::gameobject_ptr instance, int object_instance_id, int instance_super_type, int instance_sub_type) {
        this->instance_ptr        = instance;
		this->network_instance_id = object_instance_id;
		this->instance_type_group = instance_super_type;
		this->instance_type       = instance_sub_type;
	}

    gameobject::gameobject_ptr INetworkInstance::getInstance() {
        return this->instance_ptr;
    }

    INetworkInstance::INetworkInstance(){}
}