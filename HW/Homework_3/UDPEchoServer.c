#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>

#define MAXPENDING 5

void DieWithError(char* errorMessage);
void HandleUDPClient(int servSocket, struct sockaddr_in echoClntAddr);

int main(int argc, char* argv[])
{
	int servSock;
	int clntSock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in echoClntAddr;
	unsigned short echoServPort;
	unsigned int clntLen;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <Server Prt>\n", argv[0]);
		exit(1);
	}

	echoServPort = atoi(argv[1]);

	if ((servSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		DieWithError("socket() failed");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htons(INADDR_ANY);
	echoServAddr.sin_port = htons(echoServPort);

	if (bind(servSock, (struct sockaddr*) & echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("bind() failed");

	for (;;)
	{
		clntLen = sizeof(echoClntAddr);

		/*if ((clntSock = accept(servSock, (struct sockaddr*) & echoClntAddr, &clntLen)) < 0)
			DieWithError("accept() failed");*/

		
		HandleUDPClient(servSock, echoClntAddr);
	}
}