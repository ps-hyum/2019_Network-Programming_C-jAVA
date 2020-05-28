#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include <fcntl.h>
#include<pthread.h>

#define RCVBUFSIZE 32
#define FILERCVBUFSIZE 1024
#define FILESNDBUFSIZE 1024

#define Nothing 37
#define FTP 00
#define EchoReq 01
#define FileUpReq 02
#define FileDownReq 03
#define LsReq 04
#define RlsReq 05
#define exitReq 06
#define EchoRep 11
#define FileAck 12
#define LsAck 14
#define RlsAck 15

#define Chatting 77
#define chatReq 55
#define OK 33
#define NO 44
#define clntReq 99
#define Trying 98
#define connectReq 97

void* thread_send(void* clientSocket);
void* thread_recv(void* clientSocket);
void DieWithError(char* errorMessage);

int partner;
char status;
int sock;
int bytesRcvd, totalBytesRcvd;
char* imsiIP;
char who[] = "W?";
char ok[] = "y";
char exitString[] = "/quit";
char myexitString[] = "//quit";
char ftpString[] = "/FTP";

int main(int argc, char* argv[])
{
	status = Nothing;
	struct sockaddr_in echoServAddr;
	unsigned short echoServPort;
	char* servIP;
	char MsgType;

	if (argc != 1) {
		fprintf(stderr, "Usage: %s <Nothing>\n", argv[0]);
		exit(1);
	}

	imsiIP = (char*)malloc(sizeof(char) * 50);
	printf("server ip : ");
	scanf("%s", imsiIP);
	servIP = imsiIP;
	printf("port : ");
	scanf("%hu", &echoServPort);

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket () failed");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);
	echoServAddr.sin_port = htons(echoServPort);

	if (connect(sock, (struct sockaddr*) & echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("connect () failed");

	char imsi_c;
	scanf("%c", &imsi_c);
	//여기까지는 수정 필요x

	//ID 할당 받기
	int clntNo;

	if ((recv(sock, &clntNo, sizeof(clntNo), 0)) < 0)
		printf("recv loginNum failed\n");
	printf("Im client %d\n", clntNo);

	pthread_t p_thread_send, p_thread_recv;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_create(&p_thread_send, &attr, thread_send, (void*)& sock);
	pthread_create(&p_thread_recv, NULL, thread_recv, (void*)& clntNo);

	int result;
	pthread_join(p_thread_recv, (void*)& result);

	exit(0);
}

void* thread_send(void* clientSocket) {
	while (1) {
		char msg;

		char* echoString;
		unsigned int echoStringLen;
		char echoBuffer[RCVBUFSIZE];

		if (status == Nothing)
			printf("press 'W?' to see clients\n");

		//printf("\tready to get String\n");
		echoString = (char*)malloc(sizeof(char) * RCVBUFSIZE);
		fgets(echoString, sizeof(RCVBUFSIZE), stdin);
		if (status == Chatting)
			printf("(is sended)\n");
		echoStringLen = strlen(echoString);
		echoString[echoStringLen - 1] = '\0';
		echoStringLen--;

		if (status == Nothing) {
			if (strcmp(echoString, who) == 0) {
				char Msg = clntReq;
				send(sock, &Msg, 1, 0);
				partner;
				scanf("%d", &partner);
				send(sock, &partner, 4, 0);
			}
		}
		else if (status == connectReq) {
			//printf("채팅 요청이 왔습니다. 수락하시겠습니까?\nYes(y) / No(n)\n");

			if (strcmp(echoString, ok) == 0) {
				msg = OK;
				send(sock, &msg, 1, 0);
				send(sock, &partner, 4, 0);
				status = Chatting;
			}
			else {
				msg = NO;
				send(sock, &msg, 1, 0);
				status = Nothing;
			}
		}
		//status = Chatting
		else {
			if ((send(sock, echoString, echoStringLen, 0) != echoStringLen))
				DieWithError("send () sent a different number of bytes than expected");
			if (strcmp(echoString, exitString) == 0) {
				printf("프로그램을 종료합니다.\n");
				close(sock);
				free(imsiIP);
				free(echoString);
				status = Nothing;
				break;
			}
		}
	}
}

void* thread_recv(void* clntNo) {
	int No = *(int*)clntNo;
	while (1) {
		//printf("\tready to get msg\n");
		char msg;
		recv(sock, &msg, 1, 0);
		if (msg == chatReq) {
			status = connectReq;
			recv(sock, &partner, 4, 0);
			printf("채팅 요청이 왔습니다. 수락하시겠습니까?\nYes(y) / No(n)\n");
		}
		else if (msg == NO) {
			status = Nothing;
		}
		else if (msg == OK) {
			status = Chatting;
		}
		else if (msg == clntReq) {
			int clients, nowNum = 0;
			printf("now log in clients are \n-> ");
			for (int i = 0; i < 4; i++) {
				recv(sock, &clients, 4, 0);
				if (clients != -1)
					if (No != clients) {
						printf("client[%d] /", clients);
						nowNum++;
					}
			}
			if (nowNum != 0) {
				printf("\nselect client chat with(0~3)\n");
				int you;
				scanf("%d", &you);
				msg = chatReq;
				send(sock, &msg, 1, 0);
				send(sock, &you, 4, 0);
			}
			else
				printf("NO CLIENTS!\n");
		}
		else {
			char* echoString;
			unsigned int echoStringLen;
			char echoBuffer[RCVBUFSIZE];

			totalBytesRcvd = 0;
			while (totalBytesRcvd < echoStringLen) {
				if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
					DieWithError("recv () failed or connection closed prematurely");

				totalBytesRcvd += bytesRcvd;
				echoBuffer[bytesRcvd] = '\0';
			}
			if (strcmp(echoBuffer, myexitString) == 0) {
				printf("채팅에서 나가셨습니다.\n");
				status = Nothing;
			}
			if (strcmp(echoBuffer, exitString) == 0) {
				printf("상대방이 채팅을 나갔습니다.\n");
				status = Nothing;
			}
			printf(echoBuffer);
			printf(" (is received)\n");
			echoBuffer[0] = '\0';
		}
	}
}
