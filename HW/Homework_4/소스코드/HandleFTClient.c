#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<fcntl.h>

#define MSGSIZE 1
#define FILEINFOSIZE 260
#define RCVBUFSIZE 1024
//define msgType
#define SNDREQ 0x01

void DieWithError(char* errorMessage);

struct fileNS
{
	char fileName[256];
	unsigned int fileSize;
	unsigned char msg;
};

void HandleFTClient(int clntSocket) {
	struct fileNS Msg;

	unsigned char msgType;
	char* fileName;
	unsigned int fileSize;
	int total = 0, rread = 0;
	char Buffer[RCVBUFSIZE];

	memset(Buffer, 0, RCVBUFSIZE);

	if ((recv(clntSocket, &Msg, MSGSIZE + FILEINFOSIZE, 0)) < 0)
		DieWithError("recv() failed");

	msgType = Msg.msg;
	if (msgType == SNDREQ) {
		fileName = Msg.fileName;
		fileSize = Msg.fileSize;
		if (send(clntSocket, &msgType, MSGSIZE, 0) != MSGSIZE)
			DieWithError("send () failed to send ACK");

		FILE* fp = fopen(fileName, "w");
		if (fp == NULL)
			DieWithError("File open error");
		
			//파일 정보 수신
			while (total < fileSize) {
				if ((rread = recv(clntSocket, Buffer, RCVBUFSIZE, 0)) < 0)
					DieWithError("recv() failed");
				total += rread;
				Buffer[rread] = 0;
				fwrite(Buffer, sizeof(char), rread, fp);
				memset(Buffer, 0, RCVBUFSIZE);
			}
		printf("%s (%d bytes) is uploaded successfully\n", fileName, fileSize);
		fclose(fp);
	}

	close(clntSocket);
}