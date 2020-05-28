#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>

#define RCVBUFSIZE 32

void DieWithError(char* errorMessage);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in echoServAddr;
	int echoServAddr_size;
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

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		DieWithError("socket () failed");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);
	echoServAddr.sin_port = htons(echoServPort);

	
	char imsi_c;
	scanf("%c", &imsi_c);
	while (1) {
		echoString = (char*)malloc(sizeof(char) * RCVBUFSIZE);
		printf("msg-> ");
		fgets(echoString, sizeof(echoBuffer), stdin);

		echoStringLen = strlen(echoString);
		echoString[echoStringLen - 1] = '\0';
		echoStringLen--;

		if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) != echoStringLen)
			DieWithError("send () sent a different number of bytes than expected");

		if (strcmp(echoString, exitString) == 0) {
			printf("프로그램을 종료합니다.\n");
			close(sock);
			free(imsiIP);
			free(echoString);
			break;
		}

		totalBytesRcvd = 0;
		printf("msg<- ");
		while (totalBytesRcvd < echoStringLen) {
			echoServAddr_size = sizeof(echoServAddr);
			if ((bytesRcvd = recvfrom(sock, echoBuffer, RCVBUFSIZE - 1, 0, (struct sockaddr*)&echoServAddr, &echoServAddr_size)) <= 0)
				DieWithError("recv () failed or connection closed prematurely");
			totalBytesRcvd += bytesRcvd;
			echoBuffer[bytesRcvd] = '\0';
			printf(echoBuffer);
		}

		printf("\n");
		echoBuffer[0] = '\0';
	}
	exit(0);

}