#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define RCVBUFSIZE 32

void DieWithError(char* errorMessage);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in echoServAddr;
	unsigned short echoServPort;
	char* servIP;
	char* echoString;
	char echoBuffer[RCVBUFSIZE];
	unsigned int echoStringLen;
	int bytesRcvd, totalBytesRcvd;

	if (argc != 1) {
		fprintf(stderr, "Usage: %s <Nothing>\n", argv[0]);
		exit(1);
	}

	char* imsiIP = (char*)malloc(sizeof(char) * 50);
	printf("server ip : ");
	scanf("%s", imsiIP);
	servIP = imsiIP;
	printf("port : ");
	scanf("%hu", &echoServPort);
	char exitString[] = "/quit";

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket () failed");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);
	echoServAddr.sin_port = htons(echoServPort);

	if (connect(sock, (struct sockaddr*) & echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("connect () failed");
	

	char* imsiString;
	while (1) {
		imsiString = (char*)malloc(sizeof(char) * RCVBUFSIZE);
		printf("msg-> ");
		scanf("%s", imsiString);
		echoString = imsiString;

		echoStringLen = strlen(echoString);

		if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
			DieWithError("send () sent a different number of bytes than expected");

		if (strcmp(echoString, exitString) == 0) {
			printf("프로그램을 종료합니다.\n");
			close(sock);
			free(imsiIP);
			free(imsiString);
			break;
		}

		totalBytesRcvd = 0;
		printf("msg<- ");
		while (totalBytesRcvd < echoStringLen) {
			if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
				DieWithError("recv () failed or connection closed prematurely");
			totalBytesRcvd += bytesRcvd;
			echoBuffer[bytesRcvd] = '\0';
			printf(echoBuffer);
			printf("\n");
		}

		echoBuffer[0] = '\0';
	}
	exit(0);

}