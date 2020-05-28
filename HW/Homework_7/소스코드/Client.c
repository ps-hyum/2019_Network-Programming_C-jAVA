#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
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
char funcFTP(int sock, char input, char MsgType, struct sockaddr_in echoServAddr);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in echoServAddr;
	unsigned short echoServPort;
	char* servIP;
	char MsgType;
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
	char ftpString[] = "/FTP";

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
	while (1) {
		echoString = (char*)malloc(sizeof(char) * RCVBUFSIZE);
		printf("msg-> ");
		fgets(echoString, sizeof(echoBuffer), stdin);

		echoStringLen = strlen(echoString);
		echoString[echoStringLen - 1] = '\0';
		echoStringLen--;

		MsgType = EchoReq;
		//if input is '/FTP' else
		if (strcmp(echoString, ftpString) == 0) {
			MsgType = FTP;
			//MsgType을 받음
			send(sock, &MsgType, 1, 0);
			recv(sock, &MsgType, 1, 0);
			printf("Welcome to Socket FTP client!\n");
			char in = 'x', input;
			while (in != 'e') {
				printf("ftp command [ p) ut\tg) et\tl) s\t r) ls\te) xit ] -> ");
				scanf("%c", &input);
				getchar();
				in = funcFTP(sock, input, MsgType, echoServAddr);
			}
		}
		//MsgType을 EchoReq로 하고 보냄
		else {
			//EchoReq를 보냄
			if ((send(sock, &MsgType, 1, 0)) < 0)
				printf("send EchoReq failed\n");
			//EchoRep를 받음
			if ((recv(sock, &MsgType, 1, 0)) < 0)
				printf("recv EchoRep failed\n");

			if (MsgType == EchoRep) {
				if ((send(sock, echoString, echoStringLen, 0) != echoStringLen))
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
					if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
						DieWithError("recv () failed or connection closed prematurely");
					totalBytesRcvd += bytesRcvd;
					echoBuffer[bytesRcvd] = '\0';
					printf(echoBuffer);
				}

				printf("\n");
				echoBuffer[0] = '\0';
			}
		}
	}
	exit(0);

}

char funcFTP(int sock, char input, char MsgType1, struct sockaddr_in echoServAddr) {
	//put
	char MsgType = MsgType1;
	switch (input) {
	case 'p': {
		MsgType = FileUpReq;
		//MsgType = FileUpReq를 보냄
		send(sock, &MsgType, 1, 0);

		char fileName[256];
		int fileSize;
		int sread, total = 0;

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
		recv(sock, &MsgType, 1, 0);

		//파일 정보 보내기			
		if (MsgType == FileAck) {
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
		return input;
	}
			  //get
	case 'g': {
		MsgType = FileDownReq;
		//메세지 타입 보내기
		send(sock, &MsgType, 1, 0);

		char fileName[256];
		unsigned int fileSize;
		int total = 0, rread = 0;
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
		MsgType = FileAck;
		send(sock, &MsgType, 1, 0);
		
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
		return input;
	}
			  //ls
	case 'l': {
		MsgType = LsReq;
		send(sock, &MsgType, 1, 0);
		recv(sock, &MsgType, 1, 0);

		DIR* dir = NULL;
		struct dirent* entry;
		char* cwd = (char*)malloc(sizeof(char) * 1024);
		getcwd(cwd, 1024);
		if ((dir = opendir(cwd)) == NULL)
			DieWithError("opendir() error");

		while ((entry = readdir(dir)) != NULL) {
			if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)){}
			else printf("%s\n", entry->d_name);
		}

		printf("\n");
		closedir(dir);
		free(cwd);
		return input;
	}
			  //rls
	case 'r': {
		MsgType = RlsReq;
		send(sock, &MsgType, 1, 0);

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

		return input;
	}
			  //exit
	case 'e': {
		MsgType = exitReq;
		send(sock, &MsgType, 1, 0);
		recv(sock, &MsgType, 1, 0);
		return input;
	}
	}
}