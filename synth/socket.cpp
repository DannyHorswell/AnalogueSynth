/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

// from http://www.linuxhowtos.org/C_C++/socket.htm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string>
#include <vector>

#include "constants.h"
#include "socket.h"
#include "patch.h"

#ifdef ENABLE_SOCKET

synth_socket::synth_socket()
{

}

synth_socket::~synth_socket()
{
	if (listensock)
	{
		close(listensock);
	}

	if (listensock)
	{
		close(newsockfd);
	}
}

void synth_socket::error(const char *msg)
{
    printf("Socket error  %s\n", msg);
}

int synth_socket::opensocket()
{
     listensock = socket(AF_INET, SOCK_STREAM, 0);

     if (listensock < 0)
	 {
        error("ERROR opening socket");
	 }

	 int on = 1;
	 int rc;

	 // ******** Set to non blocking
	 rc = ioctl(listensock, FIONBIO, (char *) &on);
   
	 if (rc < 0)
	 {
		error("ioctl() failed");
		close(listensock);
	 }

     bzero((char *) &serv_addr, sizeof(serv_addr));

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(TCP_PORT);

     if (bind(listensock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	 {
		error("ERROR on binding");
	 }

     bzero(buffer,256);
}

int synth_socket::polllisten()
{
	if (newsockfd <= 0) // Once connected we do not want to listen anymore
	{
		 int rc = listen(listensock,5);

		 if (rc < 0)
		 {
			 perror("listen failed");
			 close(listensock);
		 }

		 socklen_t clilen = sizeof(cli_addr);

		 newsockfd = accept(listensock, (struct sockaddr *) &cli_addr, &clilen);

		 switch (newsockfd)
		 {
			 case -EBADF:
				 //printf("accept returned EBADF %i\n" , newsockfd);
				 break;

			 case -EFAULT:
				 //printf("accept returned EFAULT %i\n" , newsockfd);
				 break;

			case -ENOTSOCK:
				 //printf("accept returned ENOTSOCK %i\n" , newsockfd);
				 break;

			case -EOPNOTSUPP:
				 //printf("accept returned EOPNOTSUPP %i\n" , newsockfd);
				 break;

			case -EWOULDBLOCK:
				 //printf("accept returned EWOULDBLOCK %i\n" , newsockfd);
				 break;

			default:
				//printf("accept returned %i\n" , newsockfd);
				break;
		 }

		 // ******** Set to non blocking
		 if (newsockfd > 0)
		 {
			 int on = 1;

			 rc = ioctl(newsockfd, FIONBIO, (char *) &on);
   
			 if (rc < 0)
			 {
				error("ioctl() failed");
				close(listensock);
			 }
		 }
	}


	if (newsockfd > 0)
	{
		// We have a scoket so try a read

		int n = read(newsockfd, buffer, SYNTH_SOCKET_BUFFER_SIZE);

		if (n > 0)
		{
			string commands(buffer);
			vector<string> allCommands = split(commands, "\n");

			if (tcpCommandCallback)
			{
				for (int count=0; count<allCommands.size(); count++)
				{
					string command = allCommands[count];
					tcpCommandCallback(command);
				}
			}
			else
			{
				printf("no command callback %s",buffer); // Print the command(s)
			}
		}

		if (n == 0)
		{
			printf("socket closed by client\n");
			close(newsockfd);
			newsockfd = 0;
		}
	}
}



int synth_socket::closesocket()
{
	close(listensock);
    close(newsockfd);

    return 0;
}

#endif
