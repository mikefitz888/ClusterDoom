#include "../include/network/Network.h"

//// -------------------------------------------------------------------------------- //////
// NETWORK MANAGER
NetworkManager::NetworkManager() {
	listener = new sf::TcpListener();
	port = 31655;

	sf::Socket::Status st = listener->listen(port);
	if( st != sf::Socket::Done) {
		std::cout << "NETWORK: Failed to initialise server on port: " << port << std::endl;
		release();
	} else {

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
	if ( st == sf::Socket::Done) {
		sf::IpAddress ip = new_client->getRemoteAddress();
		std::cout << "New connection received from " << ip << ":" << new_client->getRemotePort() << std::endl;

		// Handle new connection:
		newConnection(new_client, ip);

		// Prepare new client socket
		new_client = new sf::TcpSocket();

	} else if( st == sf::Socket::Error || st == sf::Socket::Disconnected ){
		std::cout << "An error occured: " << st << std::endl;
		release();
	}

	// Process existing connections
	for (NetworkClient* it : clients) {
		it->processConnection();
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

void NetworkManager::release() {
	for (NetworkClient* it : clients) {
		it->release();
	}
	listener->close();
}


//// -------------------------------------------------------------------------------- //////
// NETWORK CLIENT
NetworkClient::NetworkClient(sf::TcpSocket *socket, sf::IpAddress ip, NetworkManager* manager) {
	this->socket		  = socket;
	this->ip			  = ip;
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
	std::srand(std::time(0)*1458484);
	security_token = rand();

	// Write a packet
	send_buffer.seek(0);
	send_buffer << NetworkManager::SERVER_PACKET_TYPE::SendWelcome;
	send_buffer << security_token;
	socket->send(send_buffer.getPtr(), send_buffer.tell());

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
	Here, we are both listening for new, incomming data
	and also testing that the socket is still live.

	If the socket is not live, we will either receive
	an error flag, or a disconnect flag.

	We can capture this and gracefully remove the connection
	from the network manager.

	Otherwise, we can handle the data that has been received.
*/
void NetworkClient::listenForData() {
	std::size_t message_size;
	sf::TcpSocket::Status st = socket->receive((void*)recv_buffer.getPtr(), (std::size_t)recv_buffer.maxSize(), message_size);
	switch(st){
		case sf::TcpSocket::Done:
			// Data received
			std::cout << "[NETWORK CLIENT] Data received from " << ip << ". Size: " << message_size << " bytes!" << std::endl;
			//disconnect("Testing Force disconnect");
			break;
		case sf::TcpSocket::Error:
		case sf::TcpSocket::Disconnected:
			release();
			break;
	}
}

void NetworkClient::processConnection() {
	// Listen for incoming data
	listenForData();
}

void NetworkClient::disconnect(char* reason) {
	if (connection_state != DISCONNECTED) {
		// Send packet to gracefully disconnect
		send_buffer.seek(0);
		send_buffer << NetworkManager::SERVER_PACKET_TYPE::SendDisconnect;
		send_buffer << "You have been disconnected by the server";	// <- Message
		send_buffer << reason;	// <- Reason
		socket->send(send_buffer.getPtr(), send_buffer.tell());

		std::cout << "[NETWORK CLIENT] Client " << ip << " has been disconnected." << std::endl << "\t\tREASON: " << reason << std::endl;
		connection_state = DISCONNECTED;
		release(); // We have to call release. This may result in a double call, however it guarantees that everything gets cleaned up, no matter which function was called.
	}
}

void NetworkClient::release() {

	// Send disconnect message
	disconnect((char*) "Connection Closed By Server");

	// Disconnect socket
	socket->disconnect();
	
	// Remove connection from the Networking main array
	network_manager->removeConnection(this);
}