#ifndef _NETWORK_HEADER
#define _NETWORK_HEADER

#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
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
	enum ConnectionState { DISCONNECTED, UNVERIFIED, SECURITY, VERIFIED };
	NetworkClient(sf::TcpSocket *socket, sf::IpAddress ip, NetworkManager* manager);

	int             connection_id = 0;
	sf::TcpSocket   *socket;
	sf::IpAddress	ip;
	ConnectionState connection_state = DISCONNECTED;
	Buffer          send_buffer, recv_buffer;
	int				security_token, security_hash;
	int				timeout = 0;
	NetworkManager  *network_manager;

	// Private functions
	void setID(int connection_id);	// Used to set the unique connection identifier
	void beginSecurityProcess();	// Begins the security evaluation process for the client
	void processConnection();		// The NetworkClient can process its own connection. This is for evaluating connection state and managing the connection.
	void listenForData();			// As the NetworkClient is the link between the client and the server, it is responsible for listening to data.
	void prepareSecurityHash();		// Performs a simple operation on the security_token. This needs to be paralleled by the client.
	void disconnect(char* reason);  // Call this to force-disconnect a client.
	void release();					// Force disconnects a socket and cleans up any data.
};

/// Network Manager
class NetworkManager {
	friend class NetworkClient;

private:
	int					   port;
	sf::TcpListener		   *listener;
	sf::TcpSocket          *new_client;
	vector<NetworkClient*> clients;
	int					   connection_identifier_max = 0;

	void removeConnection(NetworkClient *network_client);

public:
	NetworkManager();
	void networkStep();
	void newConnection(sf::TcpSocket *sfc, sf::IpAddress ip);
	void release();

	/*
		This is the range of packet IDs that the server can send to the
		client.
		Every sent packet should have the ID written out at the top 
		as this will determine the structure of the future data in
		the packet.
	*/
	enum SERVER_PACKET_TYPE {
		SendWelcome           = 0x00,
		SendConfirmConnect    = 0x01,
		SendDisconnect        = 0x02,
		SendInstanceCreate    = 0x03,
		SendInstanceUpdate    = 0x04,
		SendInstanceDestroy   = 0x05,
		SendPlayerInstanceID  = 0x06
	};


	/*
		This is the range of packet IDs that the
		server can receive from the client
	*/
	enum CLIENT_PACKET_TYPE {
		recvSecurityInfo		= 0x00,
		recvDisconnect			= 0x01,
		recvInstanceInteraction = 0x02
	};

};

#endif