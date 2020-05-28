#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>

#define RCVBUFSIZE 32

void DieWithError(char *errorMessage);

void HandleTCPClient(int clntSocket){
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;

    for(int i=0; i<RCVBUFSIZE; i++)
		echoBuffer[i] = '\0';

    if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
    DieWithError("recv() failed");
    printf("msg<- %s\n", echoBuffer);

    while(recvMsgSize > 0){
        if(send(clntSocket, echoBuffer, recvMsgSize,0) != recvMsgSize)
        DieWithError("send() failed");
        printf("msg-> %s\n", echoBuffer);

        for (int i = 0; i < RCVBUFSIZE; i++)
			echoBuffer[i] = '\0';

        if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");
        printf("msg<- %s\n", echoBuffer);
    }
    close(clntSocket);
}