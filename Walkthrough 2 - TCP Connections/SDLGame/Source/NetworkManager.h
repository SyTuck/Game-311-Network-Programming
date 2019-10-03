#pragma once
#define WIN32_LEAN_AND_MEAN					//stripped down version of the windows library
#pragma comment (lib, "ws2_32.lib")			//windows socket library

#include <Windows.h>						//required for windows networking
#include <WinSock2.h>						//primary Socket API

//#define SERVERIP "10.105.11.46"			//2nd comp
//#define SERVERIP "10.105.11.53"			//Alex
//#define SERVERIP "10.105.10.38"			//J lab 2nd comp
#define SERVERIP "10.105.10.36"				//J lab Amel
//#define SERVERIP "10.105.11.56"			//Amel
#define CLIENTIP "10.105.11.49"				//main comp
#define GAMEPORT 8890

class NetworkManager
{

public:
	static NetworkManager* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new NetworkManager();
		}
		return instance;
	}

	void Init();
	void SetupSockets();
	void BindSockets();
	void SendData(const char *data);
	char* ReceiveData();

	//client methods
	void ConnectSockets();
	
	//server methods
	void AcceptConnections();

	void Shutdown();

private:
	NetworkManager();
	~NetworkManager();

	SOCKET listenSocket;
	SOCKET peerSocket;

	SOCKADDR_IN listenAddr; 
	SOCKADDR_IN peerAddr;
	
	//singleton instance
	static NetworkManager* instance;

	const int MAX_MESSAGE_SIZE = 1300;

};

