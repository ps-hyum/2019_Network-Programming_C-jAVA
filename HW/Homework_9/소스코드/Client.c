#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include <fcntl.h>
#include<pthread.h>
#include<sys/stat.h>

#define RCVBUFSIZE 32
#define FILERCVBUFSIZE 1024
#define FILESNDBUFSIZE 1024

//status
#define Waitting 99
#define Chatting 88
#define FTP 77
#define ChatReqArv 66
#define Selecting 55

//request
#define clntReq 01
#define chatReq 02
#define chatexitReq 04
#define okRep 05
#define noRep 06

#define ftpReq 07

#define pReq 34
#define gReq 35
#define lReq 36
#define rReq 37
#define cReq 38
#define mReq 39
#define dReq 40
#define rcReq 41
#define mkReq 42
#define deReq 43
#define ftpexitReq 44

#define fileACK 50

#define exitProgram 30

#define _MAX_PATH 260
#define _MAX_FILENAME 256

void ftpFunc(int sock, char* ftpString, struct sockaddr_in echoServAddr);
void* thread_send(void* clientSocket);
void* thread_recv(void* clientSocket);
void DieWithError(char* errorMessage);

int partner;
char status;
int sock;
int bytesRcvd, totalBytesRcvd;
char* imsiIP;
char clientReq[] = "W?";
char OKRep[] = "y";
char NORep[] = "n";
char exitString[] = "QUIT!";
char ftpString[] = "/FTP";
char exitClient[] = "/quit";
char p[] = "p";
char g[] = "g";
char l[] = "l";
char r[] = "r";
char c[] = "c";
char m[] = "m";
char d[] = "d";
char rc[] = "rc";
char mk[] = "mk";
char de[] = "de";
char e[] = "e";

int partnerID;
struct sockaddr_in echoServAddr;
int clntNo;

int main(int argc, char* argv[])
{
	status = Waitting;
	unsigned short echoServPort;
	//char* servIP;
	char MsgType;


	if (argc != 1) {
		fprintf(stderr, "Usage: %s <Waitting>\n", argv[0]);
		exit(1);
	}

	const char* servIP;
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

	//ID 할당 받기
	if ((recv(sock, &clntNo, sizeof(clntNo), 0)) < 0)
		printf("recv loginNum failed\n");
	printf("Welcome! : client[%d]\n", clntNo);

	pthread_t p_thread_send, p_thread_recv;

	pthread_create(&p_thread_send, NULL, thread_send, (void*)& sock);
	pthread_create(&p_thread_recv, NULL, thread_recv, (void*)& sock);

	pthread_join(p_thread_send, (void*)& sock);
	pthread_join(p_thread_recv, (void*)& sock);

	exit(0);
}

void* thread_send(void* clientSocket) {
	printf("- 'W?'   : to see clients\n");
	printf("- '/FTP' : to use FTP\n");
	while (1) {
		char* echoString;
		unsigned int echoStringLen;
		char echoBuffer[RCVBUFSIZE];

		echoString = (char*)malloc(sizeof(char) * RCVBUFSIZE);
		fgets(echoString, sizeof(echoBuffer), stdin);

		echoStringLen = strlen(echoString);
		echoString[echoStringLen - 1] = '\0';
		echoStringLen--;

		char msg;

		switch (status)
		{
		case Waitting: {
			//clntReq
			if (strcmp(echoString, clientReq) == 0) {
				msg = clntReq;
				send(sock, &msg, 1, 0);
				break;
			}
			//ftpReq
			else if (strcmp(echoString, ftpString) == 0) {
				status = FTP;
				msg = ftpReq;
				send(sock, &msg, 1, 0);
			}
			else if (strcmp(echoString, exitClient) == 0) {
				msg = exitProgram;
				send(sock, &msg, 1, 0);
				return;
			}
			else {
			}
			break;
		}
		case Selecting: {
			printf("\nSelect ID > ");
			if (strcmp(echoString, clientReq) == 0) {
				msg = clntReq;
				send(sock, &msg, 1, 0);
				break;
			}
			else if (strcmp(echoString, ftpString) == 0) {
				status = FTP;
				msg = ftpReq;
				send(sock, &msg, 1, 0);
				break;
			}
			else if (strcmp(echoString, exitClient) == 0) {
				msg = exitProgram;
				send(sock, &msg, 1, 0);
				return;
			}
			msg = chatReq;
			send(sock, &msg, 1, 0);
			int wantClnt = atoi(echoString);
			send(sock, &wantClnt, 4, 0);
			break;
		}
		case ChatReqArv: {
			if (strcmp(echoString, OKRep) == 0) {
				printf("Start chat with client[%d]\n", partnerID);
				status = Chatting;
				msg = okRep;
				send(sock, &msg, 1, 0);
				send(sock, &partnerID, 4, 0);
			}
			else if (strcmp(echoString, NORep) == 0) {
				status = Waitting;
				printf("- 'W?'   : to see clients\n");
				printf("- '/FTP' : to use FTP\n");
				msg = noRep;
				send(sock, &msg, 1, 0);
				send(sock, &partnerID, 4, 0);
			}
			break;
		}
		case Chatting: {
			if (strcmp(echoString, exitString) == 0) {
				status = Waitting;
			}
			msg = Chatting;
			send(sock, &msg, 1, 0);
			//문자열 전송
			if ((send(sock, echoString, echoStringLen, 0) != echoStringLen))
				DieWithError("send () sent a different number of bytes than expected");
			break;
		}
		}
		if (status == FTP) {
			printf("Socket FTP System!\n");
			char* ftpString;
			unsigned int ftpStringLen;
			char ftpBuffer[RCVBUFSIZE];
			ftpString = (char*)malloc(sizeof(char) * RCVBUFSIZE);
			printf("ftp command [p)ut g)et l)s r)ls c)d m)kd d)eld rc)d mk)rd de)lrd e)xit ]\n-> ");
			fgets(ftpString, sizeof(ftpBuffer), stdin);
			ftpStringLen = strlen(ftpString);
			ftpString[ftpStringLen - 1] = '\0';
			ftpStringLen--;
			while (strcmp(ftpString, e) != 0) {
				ftpFunc(sock, ftpString, echoServAddr);
				free(ftpString);
				ftpString = (char*)malloc(sizeof(char) * RCVBUFSIZE);
				printf("ftp command [p)ut g)et l)s r)ls c)d m)kd d)eld rc)d mk)rd de)lrd e)xit ]\n-> ");
				fgets(ftpString, sizeof(ftpBuffer), stdin);

				ftpStringLen = strlen(ftpString);
				ftpString[ftpStringLen - 1] = '\0';
				ftpStringLen--;
				//rs, ls 등등
			}
			free(ftpString);
			printf("exit from ftp system, bye~\n\n");
			status = Waitting;
			printf("- 'W?'   : to see clients\n");
			printf("- '/FTP' : to use FTP\n");
			msg = ftpexitReq;
			send(sock, &msg, 1, 0);
		}

		free(echoString);
	}
}

void* thread_recv(void* servSock) {
	int sock = *(int*)servSock;
	char msg;
	while (1) {
		//printf("ready to get msg\n");
		recv(sock, &msg, 1, 0);
		switch (msg)
		{
		case exitProgram: {
			return;
		}
		case clntReq: {
			status = Selecting;
			printf("now login clients : ");
			int nowClnt = -200;
			while (nowClnt != -1) {
				recv(sock, &nowClnt, 4, 0);
				if (nowClnt == -1)
					break;
				if(clntNo == nowClnt)
					printf("(client[%d]) ", nowClnt);
				else
					printf("client[%d] ", nowClnt);
			}
			break;
		}
		case chatReq: {
			recv(sock, &partnerID, 4, 0);
			printf("Chat Request if arrived from client[%d]\n", partnerID);
			printf("Yes(y), No(n)\n");
			status = ChatReqArv;
			break;
		}
		case okRep: {
			printf("client[%d] accepted Chat Request\n", partnerID);
			status = Chatting;
			break;
		}
		case noRep: {
			printf("client[%d] rejected Chat Request\n", partnerID);
			status = Waitting;
			break;
		}
		case Chatting: {
			char echoBuffer[RCVBUFSIZE];
			for (int i = 0; i < RCVBUFSIZE; i++)
				echoBuffer[i] = '\0';
			int recvMsgSize;
			if ((recvMsgSize = recv(sock, echoBuffer, RCVBUFSIZE, 0)) < 0)
				printf("failed recv String from clntSock");

			printf("msg <- ");
			printf(echoBuffer);
			printf("\n");
			break;
		}
		case chatexitReq: {
			printf("client[%d] leave the chatting\n", partnerID);
			status = Waitting;
			break;
		}
		case ftpReq: {
			status = FTP;
			while (status == FTP) {}
			break;
		}
		}
	}
}

void ftpFunc(int sock, char* ftpString, struct sockaddr_in echoServAddr) {
	char ftpMsg;

	char fileName[256];
	int fileSize;
	int total = 0, rread = 0, sread = 0;
	char* subString;
	unsigned int subStringLen;
	//put
	if (strcmp(ftpString, p) == 0) {
		ftpMsg = pReq;
		send(sock, &ftpMsg, 1, 0);

		//fileName 입력
		printf("filename to put to server -> ");
		scanf("%s", fileName);
		getchar();

		//file 열기 및 사이즈 계산
		FILE* fp = fopen(fileName, "r");
		if (fp == NULL)
			DieWithError("File open error");
		fseek(fp, 0, SEEK_END);
		fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		//파일 이름 보내기
		send(sock, fileName, 256, 0);
		//파일 사이즈 보내기
		send(sock, &fileSize, sizeof(fileSize), 0);
		//ACK받기
		recv(sock, &ftpMsg, 1, 0);

		//파일 정보 보내기			
		if (ftpMsg == fileACK) {
			char Buffer[FILESNDBUFSIZE];
			printf("Sending => ");
			while (1) {
				sread = fread(Buffer, sizeof(char), FILESNDBUFSIZE, fp);
				send(sock, Buffer, sread, 0);
				printf("#");
				if (feof(fp)) {
					printf("\n%s (%d bytes) uploading success to %s\n", fileName, fileSize, inet_ntoa(echoServAddr.sin_addr));
					fclose(fp);
					break;
				}
			}
		}
		else
			printf("received no ACK\n");
		return;
	}
	//get
	else if (strcmp(ftpString, g) == 0) {
		ftpMsg = gReq;
		send(sock, &ftpMsg, 1, 0);

		char Buffer[FILERCVBUFSIZE];

		printf("filename to get to server -> ");
		scanf("%s", fileName);
		getchar();

		//파일 이름 보내기
		send(sock, &fileName, 256, 0);

		//파일 열기(파일 이미 알고 있음)
		FILE* fp = fopen(fileName, "w");
		if (fp == NULL)
			DieWithError("File open error");

		//파일 사이즈 받기
		recv(sock, &fileSize, sizeof(fileSize), 0);

		//fileACK 보내기
		ftpMsg = fileACK;
		send(sock, &ftpMsg, 1, 0);

		printf("Receving => ");
		//파일 정보 수신
		while (total < fileSize) {
			if ((rread = recv(sock, Buffer, FILERCVBUFSIZE, 0)) < 0)
				DieWithError("recv() failed");
			total += rread;
			printf("#");
			Buffer[rread] = 0;
			fwrite(Buffer, sizeof(char), rread, fp);
			memset(Buffer, 0, FILERCVBUFSIZE);
		}
		printf("\n%s (%d bytes) is uploaded successfully\n", fileName, fileSize);
		fclose(fp);
		return;
	}
	//ls
	else if (strcmp(ftpString, l) == 0) {
		ftpMsg = lReq;
		send(sock, &ftpMsg, 1, 0);
		recv(sock, &ftpMsg, 1, 0);

		DIR* dir = NULL;
		struct dirent* entry;
		char* cwd = (char*)malloc(sizeof(char) * 1024);
		getcwd(cwd, 1024);
		if ((dir = opendir(cwd)) == NULL)
			DieWithError("opendir() error");

		while ((entry = readdir(dir)) != NULL) {
			if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {}
			else printf("%s\n", entry->d_name);
		}

		printf("\n");
		closedir(dir);
		free(cwd);
		return;
	}
	//rls
	else if (strcmp(ftpString, r) == 0) {
		ftpMsg = rReq;
		send(sock, &ftpMsg, 1, 0);

		DIR* dir = NULL;
		struct dirent* entry;
		char* cwd = (char*)malloc(sizeof(char) * 1024);
		char* mycwd = (char*)malloc(sizeof(char) * 1024);
		getcwd(mycwd, 1024);
		recv(sock, cwd, 1024, 0);
		if ((dir = opendir(cwd)) == NULL)
			DieWithError("opendir() error");

		while ((entry = readdir(dir)) != NULL) {
			if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {}
			else printf("%s\n", entry->d_name);
		}

		printf("\n");
		closedir(dir);
		free(mycwd);
		free(cwd);
		return;
	}
	else if (strcmp(ftpString, c) == 0) {
		char cdBuffer[_MAX_PATH];
		subString = (char*)malloc(sizeof(char) * _MAX_PATH);
		ftpMsg = cReq;
		send(sock, &ftpMsg, 1, 0);
		recv(sock, &ftpMsg, 1, 0);

		printf("change directory -> cd ");
		fgets(subString, sizeof(cdBuffer), stdin);
		subStringLen = strlen(subString);
		subString[subStringLen - 1] = '\0';
		subStringLen--;

		int result = chdir(subString);

		if (result == 0) {}
		else if (result == -1) {
			printf("no such directory\n");
		}

		free(subString);
	}
	else if (strcmp(ftpString, m) == 0) {
		char mkBuffer[_MAX_FILENAME];
		subString = (char*)malloc(sizeof(char) * _MAX_FILENAME);
		ftpMsg = mReq;
		send(sock, &ftpMsg, 1, 0);
		recv(sock, &ftpMsg, 1, 0);

		printf("directory name to make -> ");
		fgets(subString, sizeof(mkBuffer), stdin);
		subStringLen = strlen(subString);
		subString[subStringLen - 1] = '\0';
		subStringLen--;

		int result = mkdir(subString, 0755);

		//if (result == 0)
		//	printf("폴더 생성 성공");
		//else if (result == -1)
		//	printf("폴더 생성 실패 - 폴더가 이미 있거나 부정확함\n");

		free(subString);
	}
	else if (strcmp(ftpString, d) == 0) {
		char mkBuffer[_MAX_FILENAME];
		subString = (char*)malloc(sizeof(char) * _MAX_FILENAME);
		ftpMsg = dReq;
		send(sock, &ftpMsg, 1, 0);
		recv(sock, &ftpMsg, 1, 0);

		printf("delete directory -> ");
		fgets(subString, sizeof(mkBuffer), stdin);
		subStringLen = strlen(subString);
		subString[subStringLen - 1] = '\0';
		subStringLen--;

		int k = chdir(subString);
		if (k == 0) {
			DIR* dir = NULL;
			struct dirent* entry;
			char* cwd = (char*)malloc(sizeof(char) * 1024);
			getcwd(cwd, 1024);
			if ((dir = opendir(cwd)) == NULL)
				DieWithError("opendir() error");

			int flag = -1;
			while ((entry = readdir(dir)) != NULL) {
				if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {}
				else {
					flag = 1;
					break;
				}
			}

			int result;
			if (flag == 1) {
				printf("\"%s\" directory not empty, delete it? (y/n) -> ", subString);
				char c;
				scanf("%c", &c);
				char imsi;
				scanf("%c", &imsi);
				if (c == 'y') {
					struct dirent* entry1;
					if ((dir = opendir(cwd)) == NULL)
						DieWithError("opendir() error");

					char* strArr[256];
					int num = 0;
					while ((entry1 = readdir(dir)) != NULL) {
						if ((strcmp(entry1->d_name, ".") == 0) || (strcmp(entry1->d_name, "..") == 0)) {}
						else {
							strArr[num++] = entry1->d_name;
							result = rmdir(entry1->d_name);
							result = remove(entry1->d_name);
						}
					}
					chdir("..");
					result = rmdir(subString);
					if (result == 0) {
						printf("success delete directory!\n");
						num = 0;
						while (strArr[num] != NULL) {
							printf("%s ", strArr[num++]);
						}
						printf("\n");
					}
					else
						printf("failed delete directory.\n");
				}
				else if (c == 'n') {
					printf("not delete directory!\n");
					chdir("..");
				}
				free(cwd);
			}
			else {
				result = rmdir(subString);
				if (result == 0)
					printf("success delete directory!\n");
				else
					printf("failed delete directory.\n");
			}
		}
		else {
			printf("no such directory\n");
		}
	}
	else if (strcmp(ftpString, rc) == 0) {
		char cdBuffer[_MAX_PATH];
		subString = (char*)malloc(sizeof(char) * _MAX_PATH);
		ftpMsg = rcReq;
		send(sock, &ftpMsg, 1, 0);
		recv(sock, &ftpMsg, 1, 0);

		printf("change remote directory -> cd ");
		fgets(subString, sizeof(cdBuffer), stdin);
		subStringLen = strlen(subString);
		subString[subStringLen - 1] = '\0';
		subStringLen--;

		if ((send(sock, subString, subStringLen, 0) != subStringLen))
			DieWithError("send () sent a different number of bytes than expected");

		int result;
		recv(sock, &result, 4, 0);
		if (result == 0) {}
		else {
			printf("no such directory\n");
		}
		free(subString);
	}
	else if (strcmp(ftpString, mk) == 0) {
	   	char mkBuffer[_MAX_FILENAME];
		subString = (char*)malloc(sizeof(char) * _MAX_FILENAME);
		ftpMsg = mkReq;
		send(sock, &ftpMsg, 1, 0);
		recv(sock, &ftpMsg, 1, 0);

		printf("directory name to remote make -> ");
		fgets(subString, sizeof(mkBuffer), stdin);
		subStringLen = strlen(subString);
		subString[subStringLen - 1] = '\0';
		subStringLen--;

		if ((send(sock, subString, subStringLen, 0) != subStringLen))
			DieWithError("send () sent a different number of bytes than expected");

		recv(sock, &ftpMsg, 1, 0);
		free(subString);
	}
	else if (strcmp(ftpString, de) == 0) {
		ftpMsg = deReq;
		send(sock, &ftpMsg, 1, 0);
		recv(sock, &ftpMsg, 1, 0);
		char deBuffer[_MAX_FILENAME];
		subString = (char*)malloc(sizeof(char) * _MAX_FILENAME);

		printf("delete remote directory -> ");
		fgets(subString, sizeof(deBuffer), stdin);
		subStringLen = strlen(subString);
		subString[subStringLen - 1] = '\0';
		subStringLen--;

		if ((send(sock, subString, subStringLen, 0) != subStringLen))
			DieWithError("send () sent a different number of bytes than expected");
		int flag;
		recv(sock, &flag, 4, 0);
		if (flag == 0) {
			recv(sock, &flag, 4, 0);
			if (flag == 1) {
				printf("\"%s\" directory not empty, delete it? (y/n) -> ", subString);
				char c;
				scanf("%c", &c);
				char imsi;
				scanf("%c", &imsi);
				if (c == 'y') {
					flag = 1;
					send(sock, &flag, 4, 0);
					recv(sock, &flag, 4, 0);
					if (flag == 0)
						printf("success delete directory!\n");
					else
						printf("failed delete directory.\n");
				}
				else if (c == 'n') {
					flag = -1;
					send(sock, &flag, 4, 0);
					recv(sock, &flag, 4, 0);
					printf("not delete directory!\n");
				}
			}
			else {
				recv(sock, &flag, 4, 0);
				if (flag == 0)
					printf("success delete directory!\n");
				else
					printf("failed delete directory.\n");
			}
		}
		else {
			printf("no such directory\n");
		}
	}
	else {
		printf("\tsomething is wrong\n");
	}
}