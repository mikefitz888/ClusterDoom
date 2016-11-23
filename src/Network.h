#ifndef _NETWORK_HEADER
#define _NETWORK_HEADER

#include "SFML/Network/TcpListener.hpp"
#include "SFML/Network/TcpSocket.hpp"
#include "SFML/Network/IpAddress.hpp"
#include "SFML/Network/Packet.hpp"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <vector>
#include "Buffer.h"

using std::vector;

/// Network Client
class NetworkClient {
	friend class NetworkManager;

private:
	enum ConnectionState { UNVERIFIED, SECURITY, VERIFIED };
	NetworkClient(sf::TcpSocket *socket, sf::IpAddress ip);

	sf::TcpSocket   *socket;
	sf::IpAddress	ip;
	ConnectionState cs;
	Buffer          send_buffer;
	int				security_token;
	int				timeout = 0;

	void beginSecurityProcess();	// Begins the security evaluation process for the client
	void processConnection();		// The NetworkClient can process its own connection. This is for evaluating connection state and managing the connection.
	void listenForData();			// As the NetworkClient is the link between the client and the server, it is responsible for listening to data.


};

/// Network Manager
class NetworkManager {
	int port;
	sf::TcpListener *listener;
	sf::TcpSocket   *new_client;
	vector<NetworkClient*> clients;

public:
	NetworkManager();
	void networkStep();
	void newConnection(sf::TcpSocket *sfc, sf::IpAddress ip);
	void release();

	enum SERVER_PACKET_TYPE {
		SendWelcome           = 0x00,
		SendConfirmConnect    = 0x01,
		SendDisconnect        = 0x02,
		SendInstanceCreate    = 0x03,
		SendInstanceUpdate    = 0x04,
		SendInstanceDestroy   = 0x05,
		SendPlayerInstanceID  = 0x06
	};

};

#endif