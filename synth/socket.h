
#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <string>
#include <vector>

using namespace std;

#ifdef ENABLE_SOCKET

#define SYNTH_SOCKET_BUFFER_SIZE 256

typedef void (*TCPCommand)(const string& command);

class synth_socket
{
	int listensock;
	int newsockfd;
	char buffer[SYNTH_SOCKET_BUFFER_SIZE];
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	void error(const char *msg);

public:
	synth_socket();
	~synth_socket();

	int opensocket();
	int polllisten();

	int closesocket();

	TCPCommand tcpCommandCallback;
};
#endif

#endif