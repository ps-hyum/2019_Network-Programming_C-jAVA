#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

#define MSGSIZE 1
#define FILEINFOSIZE 260
#define FILESNDBUFSIZE 1024
//define msgType
#define SNDREQ 0x01

//파일 이름, 사이즈를 보내는 구조체 정의
struct fileNS
{
	char fileName[256];
	unsigned int fileSize;
	unsigned char msg;
};

void DieWithError(char* errorMessage);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in echoServAddr;
	unsigned short echoServPort;
	char* servIP;

	unsigned char msgType;
	char* fileName;
	int fileNameLen;
	int fileSize;
	int sread, total = 0;
	char Buffer[FILESNDBUFSIZE];

	if (argc != 4) {
		fprintf(stderr, "Usage: %s <Server IP> <Port> <File Name>]\n", argv[0]);
		exit(1);
	}

	servIP = argv[1];
	echoServPort = atoi(argv[2]);
	fileName = argv[3];

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket () failed");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);
	echoServAddr.sin_port = htons(echoServPort);

	FILE* fp = fopen(fileName, "r");
	if (fp == NULL)
		DieWithError("File open error");

	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	struct fileNS fMsg;
	strcpy(fMsg.fileName, fileName);
	fMsg.fileSize = fileSize;
	fMsg.msg = SNDREQ;

	if (connect(sock, (struct sockaddr*) & echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("connect () failed");

	//파일 이름, 사이즈 보내기
	if (send(sock, &fMsg, MSGSIZE + FILEINFOSIZE, 0) != (MSGSIZE + FILEINFOSIZE))
		DieWithError("send () sent a different number of bytes than expected");	

	//여기에 ACK를 받아야 함
	//나중에 if로 나누어서 할 작업을 정해줄 수 있음
	while (msgType != 0x01) {
		recv(sock, &msgType, MSGSIZE, 0);
	}
	printf("Sending => ");

	//파일 정보 보내기
	while (1) {
		sread = fread(Buffer, sizeof(char), FILESNDBUFSIZE, fp);
		send(sock, Buffer, sread, 0);
		printf("#");
		if (feof(fp)) {
			printf("\n");
			fclose(fp);
			break;
		}
	}
	printf("%s (%d bytes) uploading success to %s\n", fMsg.fileName, fileSize, inet_ntoa(echoServAddr.sin_addr));
	
	exit(0);
}