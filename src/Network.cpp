#include "Network.h"

//// -------------------------------------------------------------------------------- //////
// NETWORK MANAGER
NetworkManager::NetworkManager() {
	listener = new sf::TcpListener();
	port = 31655;

	if (listener->listen(port) != sf::Socket::Done) {
		std::cout << "NETWORK: Failed to initialise server on port: " << port << std::endl;
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
	if (listener->accept(*new_client) == sf::Socket::Done) {
		sf::IpAddress ip = new_client->getRemoteAddress();
		std::cout << "New connection received from " << ip << std::endl;

		// Handle new connection:
		newConnection(new_client, ip);

		// Prepare new client hook
		new_client = new sf::TcpSocket();
	}

	// Process existing connections
	for (NetworkClient* it : clients) {
		it->processConnection();
	}

}

void NetworkManager::newConnection(sf::TcpSocket *sfc, sf::IpAddress ip) {
	NetworkClient *new_client = new NetworkClient(sfc, ip);
	clients.push_back(new_client);

	// Begin security process on new client
	new_client->beginSecurityProcess();
}

void NetworkManager::release() {
	listener->close();
}


//// -------------------------------------------------------------------------------- //////
// NETWORK CLIENT
NetworkClient::NetworkClient(sf::TcpSocket *socket, sf::IpAddress ip) {
	this->socket = socket;
	this->ip = ip;
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
}

void NetworkClient::listenForData() {

}

void NetworkClient::processConnection() {
	std::cout << "processing connection" << std::endl;
}