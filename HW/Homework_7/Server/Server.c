#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

#define MAXPENDING 5

void DieWithError(char* errorMessage);
void HandleClient(int clntSocket);
void* thread_func(void* clientSocket);

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

	if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htons(INADDR_ANY);
	echoServAddr.sin_port = htons(echoServPort);

	if (bind(servSock, (struct sockaddr*) & echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("bind() failed");

	if (listen(servSock, MAXPENDING) < 0)
		DieWithError("listen() failed");

	pthread_t p_thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	for (;;)
	{
		clntLen = sizeof(echoClntAddr);

		if ((clntSock = accept(servSock, (struct sockaddr*) & echoClntAddr, &clntLen)) < 0)
			DieWithError("accept() failed");

		printf("client ip :  %s\n", inet_ntoa(echoClntAddr.sin_addr));
		printf("port : %d\n", htons(echoClntAddr.sin_port));

		pthread_create(&p_thread, &attr, thread_func, (void*)& clntSock);
	}
}

void* thread_func(void* clientSocket) {
	int clntSock = *(int*)clientSocket;
	HandleClient(clntSock);
	close(clntSock);
}