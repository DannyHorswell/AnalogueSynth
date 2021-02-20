#ifdef _WIN32
#include <winsock.h>
#include <errno.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")
#endif

#ifdef __arm__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#endif
#include <thread>

#include "constants.h"
#include "socket.h"


using namespace std;


SocketClient::SocketClient()
{

}

void SocketClient::Initalise(string serverIp, int port)
{
	printf("SocketClient::Initalise \n");

	_serverIp = serverIp;
	_serverPort = port;
}

void SocketClient::Connect()
{
	printf("SocketClient::Connect %s:%d \n", _serverIp.c_str(), _serverPort);

	connectionThread = new thread(&SocketClient::ConnectLoop, this);
}


void SocketClient::Disconnect()
{
	printf("SocketClient::Disconnect \n");

	shutdown = true;
}

bool SocketClient::DiscoverEndpoint()
{
	printf("Game engine discovery\n");

	int iResult = 0;

	if (discoverSocket == 0)
	{
		discoverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		// Set discovery timeout
#ifdef _WIN32
		DWORD timeout = 10 * 1000;
		setsockopt(discoverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);
#endif

#ifdef __arm__
		struct timeval tv;
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		setsockopt(discoverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
#endif

		int broadcastPermission = 1;					/* Socket opt to set permission to broadcast */


		iResult = setsockopt(discoverSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcastPermission, sizeof(broadcastPermission));

		if (iResult < 0)
		{
			printf("setsockopt error %d\n", iResult);
			return false;
		}
	}

	const char* sendString = "?? Game Engine";      /* String to broadcast */
	unsigned int sendStringLen;						/* Length of string to broadcast */
	struct sockaddr_in broadcastAddr; /* Broadcast address */

	/* Construct local address structure */
	memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
	broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
	broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");/* Broadcast IP address */
	broadcastAddr.sin_port = htons(NETWORK_DISCOVERY_PORT);         /* Broadcast port */

	sendStringLen = strlen(sendString);  /* Find length of sendString */

	// Broadcast message
	iResult = sendto(discoverSocket, sendString, sendStringLen, 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

	if (iResult != sendStringLen)
	{
		printf("sendto incorrect length sent %d\n", iResult);
		return false;
	}

	// Receive response
	iResult = recv(discoverSocket, readBuffer, READ_BUFFER_SIZE, 0);

	if (iResult > 0)
	{
		readBuffer[iResult] = 0; // Terminate the string in the buffer
		printf("Discovery received %s\n", readBuffer);

		string responseString(readBuffer);

		if (responseString.rfind(GAME_ENGINE_DISCOVERY_RESPONSE_START, 0) == 0) {

			responseString = responseString.substr(strlen(GAME_ENGINE_DISCOVERY_RESPONSE_START));

			int colonLocation = responseString.find(":");

			string ip = responseString.substr(0, colonLocation);
			responseString = responseString.substr(colonLocation + 1);

			colonLocation = responseString.find(":");
			string port = responseString.substr(0, colonLocation);

			printf("Server address %s:%s\n", ip.c_str(), port.c_str());

			_serverIp = ip;
			_serverPort = stoi(port);

			return true;
		}
		else
		{
			printf("Unrecognised discovery response %d\n", iResult);
			return false;
		}
	}
	else
	{
		printf("Discovery error %d\n", iResult);
		return false;
	}
}

void SocketClient::ConnectLoop()
{
#ifdef _WIN32
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);

	int startupResult = WSAStartup(wVersionRequested, &wsaData);

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		return;
	}
#endif

	int iResult;

	do
	{
		bool discovered = false;

		while (!discovered)
		{
			discovered = DiscoverEndpoint();
		}

		sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		sockaddr_in address;

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr(_serverIp.c_str());
		address.sin_port = htons(_serverPort);

		printf("Try connect to %s:%d \n", _serverIp.c_str(), _serverPort);
		int iResult = connect(sockfd, (struct sockaddr*)&address, sizeof(address));

		if (iResult < 0)
		{
			printf("Connection error: %d \n", iResult);
		}
		else
		{
			printf("Socket connected \n");
			ReadingLoop();
		}
	} while (!shutdown);

#ifdef ADDITIONAL_LOGGING
	printf("Connection shutdown true\n");
#endif
}

void SocketClient::ReadingLoop()
{
	printf("Entering SocketClient::ReadingLoop \n");

	int iResult;

	// Receive until the peer closes the connection
	do {

		try
		{
			iResult = recv(sockfd, readBuffer, READ_BUFFER_SIZE, 0);

			if (iResult > 0)
			{
				readBuffer[iResult] = 0; // Terminate the string in the buffer
				//_pModel->HandleCommand(string(readBuffer));
			}
			else if (iResult == 0)
			{
				printf("Connection closed\n");
				sockfd = 0;
			}
			else
			{
				printf("recv failed: %s (%d)\n", strerror(errno), errno);
				sockfd = 0;
			}
		}
		catch (exception ex)
		{
			printf("Exception reading \n");
		}

	} while (iResult > 0 && !shutdown);

	printf("Exit reading loop \n");
}

void SocketClient::SendString(string message)
{
	if (sockfd != 0)
	{
		const char* csmessage = message.c_str();
		int result = send(sockfd, csmessage, strlen(csmessage), 0);

		if (result < 0)
		{
			//int error = WSAGetLastError();

			//bool berr = true;
		}
	}
}
