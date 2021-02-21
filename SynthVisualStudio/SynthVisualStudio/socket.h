#pragma once
#include <string>
#include <functional>
#include <thread>

class PhoneModel;

#define READ_BUFFER_SIZE	1024
#define WRITE_BUFFER_SIZE	1024

using namespace std;

//using stringReceivedCallback = void(*)(string);

class SocketClient
{
protected:
	string _serverIp;
	int _serverPort;

	int discoverSocket = 0;

	int sockfd = 0;

	char readBuffer[READ_BUFFER_SIZE] = { 0 };
	char writeBuffer[WRITE_BUFFER_SIZE] = { 0 };

	bool DiscoverEndpoint();
	void ConnectLoop();
	void ReadingLoop();

	thread* connectionThread = nullptr;
	bool shutdown = false;

public:
	SocketClient();

	void Initalise(string serverIp, int port);

	void Connect();
	void Disconnect();


	void SendString(string message);
};

