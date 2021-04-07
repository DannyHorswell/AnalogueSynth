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

extern int SocketCommand(const string& com);


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

	printf("SocketServer::Open %d \n", _ListenPort);

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

	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_ListenPort);

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

	socklen_t addrlen = sizeof(clientaddr);
	clientScokFd = accept(listenSockfd, (struct sockaddr *)&clientaddr, &addrlen);

    if (clientScokFd<0)
    {
		printf("Accept %d \n", _ListenPort);
        perror("accept");
        exit(EXIT_FAILURE);
    }

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
				SocketCommand(string(readBuffer));
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
