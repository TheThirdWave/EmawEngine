#include "Lobby.h"

// Default Constructor
Lobby::Lobby()
{
}

// Constructor that assigns the id and has a reference to the network. This constructor is preferred.
Lobby::Lobby(uint id, ServerNetwork* network)
{
	m_id = id;
	m_network = network;
}

// Default destructor.
Lobby::~Lobby()
{
}

// Gets the Lobby's ID.
uint Lobby::getId() {
	return m_id;
}

// Gets the number of players in the lobby.
u8 Lobby::getPlayerCount() {
	return m_playerCount;
}

// Add's a client to the lobby and stores the client's ID in the list.
uint Lobby::addClient(uint clientId) {
	m_playerIds.push_back(clientId);
	m_playerCount++;
	if (m_playerCount == 1)
		startGame();
	return m_id;
}

// Start's the game attached to this lobby.
void Lobby::startGame() {
	std::cout << "game has started\n";
}

// Checks all client's for messages, updates the game state, and then broadcasts it back to all clients.
void Lobby::update() {
	receiveFromClients();
	m_game->update();
	sendUpdateToAll();
}

// Checks for messages from all clients.
void Lobby::receiveFromClients()
{
	// go through all clients
	std::vector<uint>::iterator iter;
	//printf("size: %i\n", m_playerIds.size());
	for (iter = m_playerIds.begin(); iter != m_playerIds.end(); iter++)
	{
		int data_length = m_network->receiveData(*iter, network_data);
		
		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}
		unsigned int * type = (unsigned int *)network_data;
		switch (*type) {

		case CONNECTION_PACKET:
			handleConnectionPacket(network_data);
			break;

		case CLIENT_UPDATE:
			handleClientUpdatePacket(network_data);
			break;

		default:
			printf("error in packet type, packet type received: %i\n", *type);
			break;
		}
	}
}

// Handles a connection packet
void Lobby::handleConnectionPacket(char * data) {
	ConnectionPacket packet = ConnectionPacket(data);
	if (packet.getType() == INIT_CONNECTION)
		printf("init packet received");
	else if (packet.getType() == PING_CONNECTION)
		printf("ping received");
}

// Handles a client update packet
void Lobby::handleClientUpdatePacket(char * data) {
	ClientUpdatePacket packet = ClientUpdatePacket(data);
	packet.printAll();
}

// Sends an updateServerPacket to all clients.
void Lobby::sendUpdateToAll()
{
	ServerUpdatePacket packet = ServerUpdatePacket();
	Vector3 pos1; pos1.x = 1; pos1.y = 2; pos1.z = 3;
	Vector3 pos2; pos2.x = -1; pos2.y = -2; pos2.z = -3;
	ClientStateMin c, d;
	c.init(0, pos1, pos2, 0);
	d.init(1, pos2, pos1, 0);
	packet.addPlayer(c);
	packet.addPlayer(d);

	char * packet_data = packet.pack();
	int packet_size = packet.size();

	std::vector<uint>::iterator iter;
	for (iter = m_playerIds.begin(); iter != m_playerIds.end(); iter++)
	{
		m_network->sendToId(*iter, packet_data, packet_size);
	}
}