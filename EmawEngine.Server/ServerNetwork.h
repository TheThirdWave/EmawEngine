// James Tyson
#pragma once

#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <map>

#include "NetworkServices.h"

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "6881" 

class ServerNetwork
{

public:

	ServerNetwork(void);
	~ServerNetwork(void);

	// Socket to listen for new connections
	SOCKET ListenSocket;

	// Socket to give to the clients
	SOCKET ClientSocket;

	// for error checking return values
	int iResult;

	// table to keep track of each client's socket
	std::map<unsigned int, SOCKET> sessions;

	bool acceptNewClient(unsigned int & id);

	// receive incoming data
	int receiveData(unsigned int client_id, char * recvbuf);

	// send data to all clients
	void sendToAll(char * packets, int totalSize);
	void sendToId(uint id, char * data, int size);
};