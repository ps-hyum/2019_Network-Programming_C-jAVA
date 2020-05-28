#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<malloc.h>
#include<dirent.h>
#include<pthread.h>

#define RCVBUFSIZE 32
#define FILERCVBUFSIZE 1024
#define FILESNDBUFSIZE 1024

#define Nothing 255
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

void* thread_sendTo(void* clientSocket);
void DieWithError(char* errorMessage);

typedef struct connection {
	int me;
	int you;
}connectt;

void HandleClient(int clntSock, int* clntArr, int clntNo) {
	int partner;
	char status = Nothing;

	send(clntSock, &clntNo, 4, 0);
	while (1) {
		char msg;
		recv(clntSock, &msg, 1, 0);
		if (msg == OK) {
			status = Chatting;
			recv(clntSock, &partner, 4, 0);
		}
		else if (msg == clntReq) {
			//printf("get clntReq from clntSock\n");
			send(clntSock, &msg, 1, 0);
			int count = 0;
			for (int i = 0; i < 4; i++) {
				if (clntArr[i] != -1 && clntNo != i) {
					count++;
					send(clntSock, &i, 4, 0);
				}
				else {
					int k = -1;
					send(clntSock, &k, 4, 0);
				}
			}
			recv(clntSock, &partner, 4, 0);
			if (clntArr[partner] != -1) {
				msg = chatReq;
				send(clntArr[partner], &msg, 1, 0);
				send(clntArr[partner], &clntNo, 4, 0);
				recv(clntArr[partner], &msg, 1, 0);
				if (msg == OK) {
					status = Chatting;
					send(clntSock, &msg, 1, 0);
				}
				else if (msg == NO) {
					send(clntSock, &msg, 1, 0);
				}
			}
		}
		else if (msg == Chatting) {
			connectt con1, con2;
			con1.me = con2.me = clntSock;
			con1.you = con2.you = clntArr[partner];

			pthread_t p_thread1, p_thread2;
			pthread_attr_t attr;
			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

			pthread_create(&p_thread1, &attr, thread_sendTo, (void*)& con1);
			pthread_create(&p_thread2, &attr, thread_sendTo, (void*)& con2);
		}
	}
}

void* thread_sendTo(void* Socket) {
	connectt con = *(connectt*)Socket;
	int me = con.me;
	int you = con.you;

	while (1) {
		char closeString[] = "/quit";
		char echoBuffer[RCVBUFSIZE];
		int recvMsgSize = 1;
		char MsgType;

		for (int i = 0; i < RCVBUFSIZE; i++)
			echoBuffer[i] = '\0';
		
		while (recvMsgSize > 0) {
			//MsgType을 받음
			if ((recv(me, &MsgType, 1, 0)) < 0)
				DieWithError("recv(MsgType) failed");

			char* echoString;
			unsigned int echoStringLen;
			char echoBuffer[RCVBUFSIZE];

			int totalBytesRcvd = 0, bytesRcvd;
			while (totalBytesRcvd < echoStringLen) {
				if ((bytesRcvd = recv(me, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
					DieWithError("recv () failed or connection closed prematurely");

				totalBytesRcvd += bytesRcvd;
				echoBuffer[bytesRcvd] = '\0';
			}

			send(you, &MsgType, 1, 0);

			echoString = (char*)malloc(sizeof(char) * RCVBUFSIZE);

			send(you, echoBuffer, RCVBUFSIZE - 1, 0);
		}
	}
}