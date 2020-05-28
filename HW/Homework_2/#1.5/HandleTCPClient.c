#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<malloc.h>

#define RCVBUFSIZE 32

void DieWithError(char* errorMessage);
void log_msg(char* msg)
{
	FILE* fp;
	fp = fopen("echo_history.log", "a");
	fprintf(fp, "%s\n", msg);
	fclose(fp);
}


void HandleTCPClient(int clntSocket) {
	char closeString[] = "/quit";
	char echoBuffer[RCVBUFSIZE];
	int recvMsgSize;
	int counter = 0;	

	for(int i=0; i<RCVBUFSIZE; i++)
		echoBuffer[i] = '\0';

	if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
		DieWithError("recv() failed");
        log_msg(echoBuffer);

	printf("msg<- %s\n", echoBuffer);

	while (recvMsgSize > 0) {
		if (counter++ != 0) {
			printf("msg<- %s\n", echoBuffer);
		}
		if (echoBuffer == closeString) {
			break;
		}
		if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
			DieWithError("send() failed");
		printf("msg-> %s\n", echoBuffer);

		for (int i = 0; i < RCVBUFSIZE; i++)
			echoBuffer[i] = '\0';

		if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
			DieWithError("recv() failed");
        log_msg(echoBuffer);
	}
	close(clntSocket);
}