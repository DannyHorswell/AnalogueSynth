

#include <thread>

#include "constants.h"
#include "socket.h"


using namespace std;

extern void SocketCommand(const string& com);


SocketServer::SocketServer()
{

}

void SocketServer::Initalise(int port)
{
	printf("SocketClient::Initalise \n");

	_ListenPort = port;
}

void SocketServer::Open()
{
	int opt = 1;

	struct sockaddr_in address;
	struct sockaddr clientaddr;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(_ListenPort);

	printf("SocketServer::Open %d \n", _ListenPort);

	

#ifdef __arm__
	// Creating socket file descriptor
	if ((listenSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
    if (setsockopt(listenSockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if (bind(listenSockfd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

	printf("Listening %d \n", _ListenPort);
	
	if (listen(listenSockfd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	//if ((clientScokFd = accept(listenSockfd, (struct sockaddr*)&address, (socklen_t*)sizeof(address))) < 0)
	if ((clientScokFd = accept(listenSockfd, NULL, NULL)) < 0)
	{
		printf("Accept %d \n", _ListenPort);
		perror("accept");
		exit(EXIT_FAILURE);
	}
#else
	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = 0;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup() failed with error: %d\n", iResult);
		return;
	}

	//----------------------
	// Create a SOCKET for listening for incoming connection requests.
	listenSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Creating socket file descriptor
	if (listenSockfd == INVALID_SOCKET)
	{
		wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
	}

	bind(listenSockfd, (SOCKADDR*)&address, sizeof(address));

	iResult = ::bind(listenSockfd, (SOCKADDR*)&address, sizeof(address));

	if (iResult == SOCKET_ERROR) {
		wprintf(L"bind function failed with error %d\n", WSAGetLastError());
		iResult = closesocket(listenSockfd);
		if (iResult == SOCKET_ERROR)
			wprintf(L"closesocket function failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	printf("Listening %d \n", _ListenPort);
	if (listen(listenSockfd, 3) == SOCKET_ERROR)
	{
		wprintf(L"listen function failed with error: %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	wprintf(L"Listening on socket...\n");

	struct sockaddr_in saClient;
	int iClientSize = sizeof(saClient);

	clientScokFd = WSAAccept(listenSockfd, (SOCKADDR*)&saClient, &iClientSize, NULL, NULL);

	if (clientScokFd == INVALID_SOCKET)
	{
		printf("Accept %d \n", _ListenPort);
		perror("accept");
		exit(EXIT_FAILURE);
	}
#endif

	SendString("HTTP/1.1 101 Switching Protocols\n");
	SendString("Upgrade: websocket\n");
	SendString("Connection: Upgrade\n");
	SendString("Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\n");
	SendString("Sec-WebSocket-Protocol: chat\n");

	ReadingLoop();
}


void SocketServer::Disconnect()
{
	printf("SocketClient::Disconnect \n");

	shutdown = true;
}


void SocketServer::ReadingLoop()
{
	printf("Entering SocketServer::ReadingLoop \n");

	int iResult;

	// Receive until the peer closes the connection
	do {

		try
		{
			iResult = recv(clientScokFd, readBuffer, READ_BUFFER_SIZE, 0);

			if (iResult > 0)
			{
				printf("Hello \n");
				readBuffer[iResult] = 0; // Terminate the string in the buffer

				string data = string(readBuffer);
				printf("Data\n");
				printf(data.c_str());
				SocketCommand(data);
			}
			else if (iResult == 0)
			{
				printf("Connection closed\n");
				clientScokFd = 0;
			}
			else
			{
				printf("recv failed: %s (%d)\n", strerror(errno), errno);
				clientScokFd = 0;
			}
		}
		catch (exception ex)
		{
			printf("Exception reading \n");
		}

	} while (iResult > 0 && !shutdown);

	printf("Exit reading loop \n");
}

void SocketServer::SendString(string message)
{
	if (clientScokFd != 0)
	{
		const char* csmessage = message.c_str();
		int result = send(clientScokFd, csmessage, strlen(csmessage), 0);

		if (result < 0)
		{
			//int error = WSAGetLastError();

			//bool berr = true;
		}
	}
}
