#include "NetworkManager.h"

#include <WS2tcpip.h>									//windows tcp/ip socket support
#include <iostream>
using namespace std;

//create our singleton instance initialization;
NetworkManager * NetworkManager::instance = nullptr;


NetworkManager::NetworkManager()
{
}

NetworkManager::~NetworkManager()
{
}

/***********************************************************************
	Init

	Initializes Windows socket API (no connection established)

************************************************************************/
void NetworkManager::Init()
{
	WSADATA wsaInfo;

	cout << "Initializing Winsock" << endl;
	int errCode = WSAStartup(MAKEWORD(2, 2), &wsaInfo);			//Initialize Winsock using version 2.2

	if (errCode != 0)
	{
		cout << "WSAStartup failed with error: " << errCode;
		return;
	}

	SetupSockets();
}

/*********************************************************************
	SetupSockets

	creates a socket and a player/peer communication socket
**********************************************************************/
void NetworkManager::SetupSockets()
{
	cout << "Setting up Socket" << endl;

	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);			//open a TCP socket for server listening (get a socket handle)

	if (listenSocket == INVALID_SOCKET)
	{
		cout << "Failed to create TCP listen socket." << endl;
		Shutdown();
	}

	peerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);				//open TCP socket for connecting with other players

	if (peerSocket == INVALID_SOCKET)
	{
		cout << "Failed to create TCP peer socket." << endl;
		Shutdown();
	}

}

/*********************************************************************
	BindSockets 	-- SERVER ONLY --

	This opens and binds the server socket
	(currently hard coded to port 8890)

**********************************************************************/
void NetworkManager::BindSockets()
{

	cout << "Binding Sockets (Server)" << endl;

	listenAddr.sin_family = AF_INET;																		//connection parameters (IPv4, port 8890, accept all incoming connections)
	listenAddr.sin_port = htons(DEFAULTGAMEPORT);
	listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenSocket, reinterpret_cast<SOCKADDR *>(&listenAddr), sizeof(listenAddr)) == SOCKET_ERROR)	//Open and bind the socket
	{
		cout << "[ERROR] TCP bind Error: " << WSAGetLastError() << endl;
		Shutdown();
	}

	listen(listenSocket, SOMAXCONN);																		//wait/listen for incoming connections
}

/*********************************************************************
	AcceptConnections --Server Only --

	This method checks for connections and reports them to the console

**********************************************************************/
void NetworkManager::AcceptConnections()
{
	int clientSize = sizeof(peerAddr);

	cout << "Accepting Sockets... (server)" << endl;

	peerSocket = accept(listenSocket, reinterpret_cast<SOCKADDR*>(&peerAddr), &clientSize);
	if (peerSocket != INVALID_SOCKET)
	{
		char ipConnected[32];
		inet_ntop(AF_INET, &peerAddr.sin_addr, ipConnected, 32);
		cout << ipConnected << "just connected into the server" << endl;	//report who connected
	}

	unsigned long b = 1;
	ioctlsocket(peerSocket, FIONBIO, &b);
}

/*********************************************************************
	ConnectSockets -- CLIENT ONLY --

	This method connects to a server at a preset port and address

**********************************************************************/
void NetworkManager::ConnectSockets()
{

	cout << "Connecting Sockets... (client)" << endl;

 	listenAddr.sin_family = AF_INET;
	listenAddr.sin_port = htons(DEFAULTGAMEPORT);
	inet_pton(AF_INET, DEFAULTSERVERIP, &listenAddr.sin_addr);

	if (connect(peerSocket, reinterpret_cast<SOCKADDR *>(&listenAddr), sizeof(listenAddr)) == SOCKET_ERROR)
	{
		cout << "[ERROR] connect Error: " << WSAGetLastError() << endl;

		Shutdown();
	}

	unsigned long b = 1;
	ioctlsocket(peerSocket, FIONBIO, &b);
}

/*********************************************************************
	SendData

	common method to both client and server. Sends data
	through the established socket connection

**********************************************************************/
void NetworkManager::SendData(const char *data)
{
	int messageLen = MAX_MESSAGE_SIZE;

	if (send(peerSocket, data, messageLen, 0) == SOCKET_ERROR)				//send the data passed (with error) checking
	{
		int error = WSAGetLastError();

		if (error != WSAEWOULDBLOCK)										////if there's an error, report it and shutdown
		{
			cout << "[Error] send error: " << WSAGetLastError() << endl;
			Shutdown();
		}
	}
}

/*********************************************************************
	ReceiveData

	This method accepts and returns incoming socket data for both
	the client and server

**********************************************************************/
char* NetworkManager::ReceiveData()
{
	int ByteReceived = 0;
	char* ReceiveBuf = new char[MAX_MESSAGE_SIZE];
	ReceiveBuf[0] = '\0';												//receive buffer with null terminator

	ByteReceived = recv(peerSocket, ReceiveBuf, MAX_MESSAGE_SIZE, 0);	//receive data (if there is any)

	if (ByteReceived == SOCKET_ERROR)									//report any errors found
	{
		int error = WSAGetLastError();									
		if (error != WSAEWOULDBLOCK)
		{
			cout << "[Error] receive error: " << WSAGetLastError() << endl;
			Shutdown();
		}
	}
	else if (ByteReceived > 0)											//report any data found
	{
		ReceiveBuf[ByteReceived] = '\0';
		cout << "TCP received data: " << ReceiveBuf << endl;
	}

	return ReceiveBuf;													//return the data received (null string if none)
}
/*********************************************************************
	Shutdown

	Method to close and release network resources

**********************************************************************/
void NetworkManager::Shutdown()
{
	cout << "about to shutdown  network socket" << endl;
	cout << "Last known error: " << WSAGetLastError() << endl;				//report any errors before shutting down

	if (peerSocket != INVALID_SOCKET)										//if we have a valid socket, close it
	{
		if (closesocket(peerSocket) != 0)
		{
			cout << "[ERROR] error closing TCP peer socket." << endl;
		}
	}

	if (listenSocket != INVALID_SOCKET)										
	{
		if (closesocket(listenSocket) != 0)
		{
			cout << "[ERROR] error closing TCP listen socket." << endl;
		}

	}

	WSACleanup();															//close resources and de-allocate socket memory
	cout << endl;
	system("pause");

	exit(0);																//immediate program close (safe here?)
}


