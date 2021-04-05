#pragma once
#ifdef _WIN32
#include <winsock2.h>
#include <stdio.h>
#include <errno.h>
#include <windows.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")
#endif

#ifdef __arm__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#endif

#include <string>
#include <functional>
#include <thread>

class PhoneModel;

#define READ_BUFFER_SIZE	1024
#define WRITE_BUFFER_SIZE	1024

using namespace std;

//using stringReceivedCallback = void(*)(string);

class SocketServer
{
protected:

	int _ListenPort;

#ifdef __arm__
	int listenSockfd = 0;
	int clientScokFd = 0;
#else
	SOCKET listenSockfd;
	SOCKET clientScokFd;
#endif

	char readBuffer[READ_BUFFER_SIZE] = { 0 };
	char writeBuffer[WRITE_BUFFER_SIZE] = { 0 };

	bool shutdown = false;

	void ReadingLoop();
	
public:
	SocketServer();

	void Initalise(int port);

	void Open();
	
	void Disconnect();


	void SendString(string message);
};

