#ifndef _NETWORK_HEADER
#define _NETWORK_HEADER

#include "../gamecore.h"
#include "Buffer.h"
#include "../smartpointers.h"

namespace network {
    using std::vector;
    using manager::Manager;
    
    /// Network Client
    class NetworkClient {
        friend class NetworkManager;

    private:
        enum ConnectionState { DISCONNECTED, UNVERIFIED, SECURITY, VERIFIED };
        explicit NetworkClient(sf::TcpSocket *socket, sf::IpAddress ip, NetworkManager* manager);

        int             connection_id = 0;
        sf::TcpSocket*  socket;
        sf::IpAddress   ip;
        ConnectionState connection_state = DISCONNECTED;
        Buffer          send_buffer, recv_buffer;
        int             security_token, security_hash;
        int             timeout = 0;
        NetworkManager* network_manager;

        // Private functions
        void setID(int connection_id);                        // Used to set the unique connection identifier
        void beginSecurityProcess();                        // Begins the security evaluation process for the client
        void completeSecurityProcess(int received_token);    // Completes the security process by comparing received token
        void processConnection(Manager* manager);                            // The NetworkClient can process its own connection. This is for evaluating connection state and managing the connection.
        void listenForData(Manager* manager);                                // As the NetworkClient is the link between the client and the server, it is responsible for listening to data.
        void prepareSecurityHash();                            // Performs a simple operation on the security_token. This needs to be paralleled by the client.
        void disconnect(char* reason);                        // Call this to force-disconnect a client.
        void sendPacket(Buffer &buff) const;                        // Sends a packet with the given Buffer to the client represented by NetworkClient
        ConnectionState getConnectionStatus() const;                // Returns the connection status of the client.
        void release();                                        // Force disconnects a socket and cleans up any data.

        // Network update functions
        void sendInstanceCreate(int instance_id, int instance_super_type, int instance_sub_type, int x, int y);
        void sendInstanceDestroy(int instance_id);
    };

    /// Network Manager
    class NetworkManager {
        friend class NetworkClient;

    private:
        int                    port;
        sf::TcpListener        *listener;
        sf::TcpSocket          *new_client;
        vector<NetworkClient*> clients;
        int                    connection_identifier_max = 0;
        Buffer                 send_buffer;
        Manager                *manager;

        void removeConnection(NetworkClient *network_client);

    public:
        NetworkManager(Manager *manager);
        void networkStep();
        void newConnection(sf::TcpSocket *sfc, sf::IpAddress ip);
        Buffer& getSendBuffer();
        void sendToAll(Buffer &buff);    // Will broadcast a packet to ALL clients who are fully connected
        void release();

        // Network update functions
        void sendInstanceCreate(NetworkClient *client, int instance_id, int instance_super_type, int instance_sub_type, int x, int y);
        void sendInstanceCreate(int instance_id, int instance_super_type, int instance_sub_type, int x, int y);
        void sendInstanceDestroy(int instance_id);
        void sendAllInstancesToClient(NetworkClient *network_client);


        /*
            This is the range of packet IDs that the server can send to the
            client.
            Every sent packet should have the ID written out at the top
            as this will determine the structure of the future data in
            the packet.
        */
        enum SERVER_PACKET_TYPE {
            SendWelcome = 0x00,
            SendConfirmConnect = 0x01,
            SendDisconnect = 0x02,
            SendInstanceCreate = 0x03,
            SendInstanceUpdate = 0x04,
            SendInstanceDestroy = 0x05,
            SendPlayerInstanceID = 0x06
        };


        /*
            This is the range of packet IDs that the
            server can receive from the client
        */
        enum CLIENT_PACKET_TYPE {
            RecvSecurityInfo = 0x00,
            RecvDisconnect = 0x01,
            RecvInstanceInteraction = 0x02
        };

    };

    /// Network Instance
    /*
        The INetworkInstance interface should be bound to all GameObjects. It represents a group of functions that allow an object to sync its state across multiple devices.
        It also provides the ability to handle instance-specific interactions which occur on the client devices.

        - The purpose of this interface is to abstract away the complexity of networking within objects themselves.




    */
    class INetworkInstance {

    private:
        /*
            network_instance_id is the unique instance identifier associated with a given object.
            instance_type_group - The global group type as defined by
        */
        int instance_type_group;
        int instance_type;
        int network_instance_id;
        NetworkManager *manager;
        gameobject::gameobject_ptr instance_ptr = nullptr;

        /*
            writeNetworkUpdate should be overriden in all cases. It is used for preparing the update data.

            Generally, this should follow a switch structure such as:

            switch( event_id ){
                  case MY_OBJECTS_LOCAL_EVENT:

                  break;
                  case UPDATE_HEALTH:

                  break;
            }

            Important Notes:
            - Event id's are local to the object class, not global to all objects. Therefore events can be defined in a classes own specific enum.
            - If enum's are used, all event id's should be clearly qualified to represent a KNOWN integer constant. This is essential for the client
            devices, as they need to know what data to read in response to an event code.

            - DO NOT CALL THE writeNetworkUpdate function directly. In order to invoke an objects own update, the:
            sendNetworkUpdate( int event_id ) function should be used. This is ESSENTIAL as it wraps the packet in a header
            which contains information about the specific kind of global network event it is, and an instance id to ensure
            the corresponding instance on the client devices responds to an update.

			--------------------------------------------------------------
			KEY CONSIDERATIONS:
			- Every object must maintain lists of its own events, these do not have to match any other events. 
			- The lists: "Interaction events" and "Update Events" correspond to the two types of network events that can affect an object.

				Interaction events - These are events triggered by the client. The client can send one of these events to the server with a 
								     packet of data. The server-side object will trigger the interaction handling event, where it can then 
									 read in the data and perform an appropriate action.

				Update events - These are events sent from the server to the client. This gives the server an opportunity to update the 
				                remote state of each object.

				- Both of these event lists must be paralleled on the server and client. Both need to implement the events in order for
				the system to function. If the server sends data that the phone ignores, this can cause other issues.

				- Objects are responsible for the frequency at which they send updates. They shouldn't send updates constantly, however
				  key events 
        */
        virtual void writeNetworkUpdate(int event_id, Buffer &buffer) = 0;
        virtual void recvNetworkInteraction(int event_id, Buffer &buffer) = 0;

    protected:
	
        // CALL sendNetworkUpdate with the given event_id to begin the update process. This will invoke writeNetworkUpdate.
        void sendNetworkUpdate(int event_id);

	public:
        INetworkInstance();
		void setNetworkManager(NetworkManager* network_manager);
		void setNetworkID(gameobject::gameobject_ptr instance, int object_instance_id, int instance_super_type, int instance_sub_type);
        gameobject::gameobject_ptr getInstance();
    };

}
#endif
