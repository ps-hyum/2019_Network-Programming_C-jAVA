#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<malloc.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define RCVBUFSIZE 32

void DieWithError(char* errorMessage);
void log_msg(char* msg)
{
	FILE* fp;
	fp = fopen("echo_history.log", "a");
	fprintf(fp, "%s\n", msg);
	fclose(fp);
}


void HandleUDPClient(int servSocket, struct sockaddr_in echoClntAddr) {
	char closeString[] = "/quit";
	char echoBuffer[RCVBUFSIZE];
	int recvMsgSize;
	int counter = 0;
	int echoClntAddr_size;

	for (int i = 0; i < RCVBUFSIZE; i++)
		echoBuffer[i] = '\0';

	echoClntAddr_size = sizeof(echoClntAddr);
	if ((recvMsgSize = recvfrom(servSocket, echoBuffer, RCVBUFSIZE, 0, (struct sockaddr*)&echoClntAddr, &echoClntAddr_size)) < 0)
		DieWithError("recv() failed");
	
    printf("client ip :  %s\n", inet_ntoa(echoClntAddr.sin_addr));
    printf("port : %d\n", htons(echoClntAddr.sin_port));
	
	printf("msg<- %s\n", echoBuffer);

	int log_len = strlen(echoBuffer);
	char* log = (char*)malloc(sizeof(log) * log_len);
	*log = 0;
	strcat(log, echoBuffer);

	while (recvMsgSize > 0) {
		if (counter++ != 0) {
           printf("client ip :  %s\n", inet_ntoa(echoClntAddr.sin_addr));
    	    printf("port : %d\n", htons(echoClntAddr.sin_port));
		    printf("msg<- %s\n", echoBuffer);
		}

		if (strcmp(echoBuffer, closeString) == 0) {
			log_msg(log);
			free(log);
			break;
		}

		int n = sendto(servSocket, echoBuffer, recvMsgSize, 0, (struct sockaddr*) & echoClntAddr, sizeof(echoClntAddr));
		if (n != recvMsgSize) 
			DieWithError("send() failed");
		
		printf("msg-> %s\n", echoBuffer);

		for (int i = 0; i < RCVBUFSIZE; i++)
			echoBuffer[i] = '\0';

		echoClntAddr_size = sizeof(echoClntAddr);
		if ((recvMsgSize = recvfrom(servSocket, echoBuffer, RCVBUFSIZE, 0, (struct sockaddr*) & echoClntAddr, &echoClntAddr_size)) < 0)
			DieWithError("recv() failed");
		log_len += strlen(echoBuffer);
		realloc(log, log_len);
		strcat(log, echoBuffer);
	}
}