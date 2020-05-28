#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<malloc.h>
#include<dirent.h>

#define RCVBUFSIZE 32
#define FILERCVBUFSIZE 1024
#define FILESNDBUFSIZE 1024

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
	int recvMsgSize = 1;
	char MsgType;

	for (int i = 0; i < RCVBUFSIZE; i++)
		echoBuffer[i] = '\0';

	int log_len;
	char* log;
	int flag = 0;

	while (recvMsgSize > 0) {
		//MsgType을 받음
		/*printf("waiting MsgType\n");*/
		if ((recv(clntSocket, &MsgType, 1, 0)) < 0)
			DieWithError("recv(MsgType) failed");
		/*printf("rcv MSG : %d\n", MsgType);*/

		if (MsgType == EchoReq) {
			MsgType = EchoRep;
			
			//EchoRep을 보냄
			if (send(clntSocket, &MsgType, 1, 0) < 0)
				printf("EchoRep send failed\n");

			if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
				DieWithError("recv() failed");

			printf("msg<- %s\n", echoBuffer);
			
			if (flag == 1) {
				log_len += strlen(echoBuffer);
				realloc(log, log_len);
				strcat(log, echoBuffer);
			}
			else {
				log_len = strlen(echoBuffer);
				log = (char*)malloc(sizeof(log) * log_len);
				*log = 0;
				strcat(log, echoBuffer);
				flag = 1;
			}

			if (strcmp(echoBuffer, closeString) == 0) {
				log_msg(log);
				free(log);
				close(clntSocket);
				break;
			}

			if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
				DieWithError("send() failed");

			printf("msg-> %s\n", echoBuffer);

			for (int i = 0; i < RCVBUFSIZE; i++)
				echoBuffer[i] = '\0';
			

		}
		else if (MsgType == FTP) {
			send(clntSocket, &MsgType, 1, 0);

			while (MsgType != exitReq) {

				recv(clntSocket, &MsgType, 1, 0);
				switch (MsgType) {
				case FileUpReq:
				{
					char fileName[256];
					unsigned int fileSize;
					int total = 0, rread = 0;
					char Buffer[FILERCVBUFSIZE];

					//파일 이름 받기
					recv(clntSocket, fileName, 256, 0);
					FILE* fp = fopen(fileName, "w");
					if (fp == NULL)
						DieWithError("File open error");

					//파일 사이즈 받기
					recv(clntSocket, &fileSize, sizeof(fileSize), 0);

					//fileACK 보내기
					MsgType = FileAck;
					send(clntSocket, &MsgType, 1, 0);

					//파일 정보 수신
					while (total < fileSize) {
						if ((rread = recv(clntSocket, Buffer, FILERCVBUFSIZE, 0)) < 0)
							DieWithError("recv() failed");
						total += rread;
						Buffer[rread] = 0;
						fwrite(Buffer, sizeof(char), rread, fp);
						memset(Buffer, 0, FILERCVBUFSIZE);
					}
					//printf("%s (%d bytes) is uploaded successfully\n", fileName, fileSize);
					fclose(fp);
					break;
				}
				case FileDownReq:
				{
					char fileName[256];
					int fileSize;
					int sread, total = 0;

					//fileName 받기
					recv(clntSocket, fileName, 256, 0);

					//file 열기 및 사이즈 계산
					FILE* fp = fopen(fileName, "r");
					if (fp == NULL)
						DieWithError("File open error");
					fseek(fp, 0, SEEK_END);
					fileSize = ftell(fp);
					fseek(fp, 0, SEEK_SET);

					//파일 사이즈 보내기
					send(clntSocket, &fileSize, sizeof(fileSize), 0);

					//ACK받기
					recv(clntSocket, &MsgType, 1, 0);

					//파일 정보 보내기			
					if (MsgType == FileAck) {
						char Buffer[FILESNDBUFSIZE];
						while (1) {
							sread = fread(Buffer, sizeof(char), FILESNDBUFSIZE, fp);
							send(clntSocket, Buffer, sread, 0);
							if (feof(fp)) {
								//printf("%s (%d bytes) uploading success to %s\n", fileName, fileSize, inet_ntoa(echoServAddr.sin_addr));
								fclose(fp);
								break;
							}
						}
					}
					break;
				}
				case LsReq:
				{
					send(clntSocket, &MsgType, 1, 0);
					break;
				}
				case RlsReq:
				{
					char* cwd = (char*)malloc(sizeof(char) * 1024);
					getcwd(cwd, 1024);

					int cwdLen = strlen(cwd);
					cwd[cwdLen] = '\0';
					send(clntSocket, cwd, cwdLen+1, 0);

					free(cwd);
					break;
				}
				case exitReq: {
					send(clntSocket, &MsgType, 1, 0);
					break;
				}
				}
			}
		}
	}
}