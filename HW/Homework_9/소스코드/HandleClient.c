#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<malloc.h>
#include<dirent.h>
#include"scheduler.h"

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

void DieWithError(char* errorMessage);
void showClnt(int clntSock);
void startClnt(int clntNo, int clntSock);
void startRoom(int clntNo1, int clntNo2);
void startFTP(int clntNo);
void exitRoom(int clntNo, int clntSock, int partnerSock);
waitClnt* findWait(int clntNo);
chatRoom* findRoom(int clntNo);
void exitFTP(int clntNo, int clntSock);
ftpClnt* findFTP(int clntNo);
void setWait(waitClnt* tmp);
void setFTP(ftpClnt* tmp);
void setRoom(chatRoom* tmp);
int deleteWait(int No);
int deleteFTP(int No);
int deleteRoom(chatRoom* tmp);
ftpClnt* createFTP(int clntNo);
waitClnt* createWait(int clntNo, int clntSock);
chatRoom* createRoom(int clntNo1, int clntNo2);

const char* exitString = "QUIT!";

void HandleClient(int clntSock, int clntID)
{
	//logIn
	startClnt(clntID, clntSock);

	int partner;
	int partnerSock;

	send(clntSock, &clntID, 4, 0);
	char msg;
	while (1) {
		recv(clntSock, &msg, 1, 0);
		switch (msg)
		{
		case exitProgram: {
			send(clntSock, &msg, 1, 0);
			break;
		}
		case clntReq: {
			send(clntSock, &msg, 1, 0);
			showClnt(clntSock);
			break;
		}
		case chatReq: {
			int want;
			recv(clntSock, &want, 4, 0);
			partner = want;
			waitClnt* tmp = findWait(partner);
			//if(tmp == NULL)
			//{}
			partnerSock = tmp->mySock;

			send(partnerSock, &msg, 1, 0);
			send(partnerSock, &clntID, 4, 0);
			break;
		}
		case okRep: {
			recv(clntSock, &partner, 4, 0);
			waitClnt* tmp = findWait(partner);
			partnerSock = tmp->mySock;
			startRoom(partner, clntID);
			send(partnerSock, &msg, 1, 0);
			break;
		}
		case noRep: {
			recv(clntSock, &partner, 4, 0);
			waitClnt* tmp = findWait(partner);
			partnerSock = tmp->mySock;
			send(partnerSock, &msg, 1, 0);
			break;
		}
		case Chatting: {
			char echoBuffer[RCVBUFSIZE];
			for (int i = 0; i < RCVBUFSIZE; i++)
				echoBuffer[i] = '\0';

			int recvMsgSize;
			if ((recvMsgSize = recv(clntSock, echoBuffer, RCVBUFSIZE, 0)) < 0)
				printf("failed recv String from clntSock");

			if (strcmp(echoBuffer, exitString) == 0) {
				msg = chatexitReq;
				send(partnerSock, &msg, 1, 0);
				exitRoom(clntID, clntSock, partnerSock);
				break;
			}

			msg = Chatting;
			send(partnerSock, &msg, 1, 0);
			if (send(partnerSock, echoBuffer, recvMsgSize, 0) != recvMsgSize)
				printf("success recv String from clntSock But failed send it to partnerSock\n");
			break;
		}
		case ftpReq: {
			startFTP(clntID);
			send(clntSock, &msg, 1, 0);
			while (1) {
				recv(clntSock, &msg, 1, 0);
				switch (msg)
				{
				case pReq: {
					char fileName[256];
					int fileSize;
					int total = 0, rread = 0;
					char Buffer[FILERCVBUFSIZE];

					//파일 이름 받기
					recv(clntSock, fileName, 256, 0);
					FILE* fp = fopen(fileName, "w");
					if (fp == NULL)
						DieWithError("File open error");

					//파일 사이즈 받기
					recv(clntSock, &fileSize, sizeof(fileSize), 0);

					//fileACK 보내기
					msg = fileACK;
					send(clntSock, &msg, 1, 0);

					//파일 정보 수신
					while (total < fileSize) {
						if ((rread = recv(clntSock, Buffer, FILERCVBUFSIZE, 0)) < 0)
							DieWithError("recv() failed");
						total += rread;
						Buffer[rread] = 0;
						fwrite(Buffer, sizeof(char), rread, fp);
						memset(Buffer, 0, FILERCVBUFSIZE);
					}
					fclose(fp);
					break;
				}
				case gReq: {
					char fileName[256];
					int fileSize;
					int sread, total = 0;

					//fileName 받기
					recv(clntSock, fileName, 256, 0);

					//file 열기 및 사이즈 계산
					FILE* fp = fopen(fileName, "r");
					if (fp == NULL)
						DieWithError("File open error");
					fseek(fp, 0, SEEK_END);
					fileSize = ftell(fp);
					fseek(fp, 0, SEEK_SET);

					//파일 사이즈 보내기
					send(clntSock, &fileSize, sizeof(fileSize), 0);

					//ACK받기
					recv(clntSock, &msg, 1, 0);

					//파일 정보 보내기			
					if (msg == fileACK) {
						char Buffer[FILESNDBUFSIZE];
						while (1) {
							sread = fread(Buffer, sizeof(char), FILESNDBUFSIZE, fp);
							send(clntSock, Buffer, sread, 0);
							if (feof(fp)) {
								fclose(fp);
								break;
							}
						}
					}
					break;
				}
				case lReq: {
					send(clntSock, &msg, 1, 0);
					break;
				}
				case rReq: {
					char* cwd = (char*)malloc(sizeof(char) * 1024);
					getcwd(cwd, 1024);

					int cwdLen = strlen(cwd);
					cwd[cwdLen] = '\0';
					send(clntSock, cwd, cwdLen + 1, 0);

					free(cwd);
					break;
				}
				case cReq: {
					send(clntSock, &msg, 1, 0);
					break;
				}
				case mReq: {
					send(clntSock, &msg, 1, 0);
					break;
				}
				case dReq: {
					send(clntSock, &msg, 1, 0);
					break;
				}
				case rcReq: {
					send(clntSock, &msg, 1, 0);

					char rcBuffer[_MAX_PATH];
					for (int i = 0; i < _MAX_PATH; i++)
						rcBuffer[i] = '\0';

					int recvMsgSize;
					if ((recvMsgSize = recv(clntSock, rcBuffer, _MAX_PATH, 0)) < 0)
						printf("failed recv String from clntSock");
					int result = chdir(rcBuffer);

					send(clntSock, &result, 4, 0);
					break;
				}
				case mkReq: {
					send(clntSock, &msg, 1, 0);
					char mkBuffer[_MAX_PATH];
					for (int i = 0; i < _MAX_PATH; i++)
						mkBuffer[i] = '\0';

					int recvMsgSize;
					if ((recvMsgSize = recv(clntSock, mkBuffer, _MAX_PATH, 0)) < 0)
						printf("failed recv String from clntSock");

					int result = mkdir(mkBuffer, 0755);

					if (result == 0)
						printf("\t생성 성공\n");
					else if (result == -1)
						perror("\t생성 실패\n");
					send(clntSock, &msg, 1, 0);
					break;
				}
				case deReq: {
					send(clntSock, &msg, 1, 0);
					char deBuffer[_MAX_FILENAME];
					for (int i = 0; i < _MAX_FILENAME; i++)
						deBuffer[i] = '\0';

					int recvMsgSize;
					if ((recvMsgSize = recv(clntSock, deBuffer, _MAX_FILENAME, 0)) < 0)
						printf("failed recv String from clntSock");

					int exist = chdir(deBuffer);
					send(clntSock, &exist, 4, 0);
					if (exist == 0) {
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
						send(clntSock, &flag, 4, 0);
						int result;
						if (flag == 1) {
							recv(clntSock, &flag, 4, 0);
							if (flag == 1) {
								struct dirent* entry1;
								if ((dir = opendir(cwd)) == NULL)
									DieWithError("opendir() error");

								int num = 0;
								while ((entry1 = readdir(dir)) != NULL) {
									if ((strcmp(entry1->d_name, ".") == 0) || (strcmp(entry1->d_name, "..") == 0)) {}
									else {
										result = rmdir(entry1->d_name);
										result = remove(entry1->d_name);
									}
								}
								chdir("..");
								result = rmdir(deBuffer);
								send(clntSock, &result, 4, 0);
							}
							else {
								chdir("..");
								send(clntSock, &flag, 4, 0);
							}

						}
						else {
							result = rmdir(deBuffer);
							send(clntSock, &result, 4, 0);
						}
					}
					else {

					}
					break;
				}
				case ftpexitReq: {
					send(clntSock, &msg, 1, 0);
					exitFTP(clntID, clntSock);
					break;
				}
				}
				if (msg == ftpexitReq) {
					break;
				}
			}
		}
		}
		if (msg == exitProgram)
			break;
	}
}


void showClnt(int clntSock) {
	waitClnt* tmp = head_wait;
	int waitting = -1;
	if (tmp == NULL) {
		send(clntSock, &waitting, 4, 0);
		//noClnt 메세지 보내고
		return;
	}
	do {
		//clntInfo 메세지를 보내고, clntNo를 보낸다.
		waitting = tmp->clntNo;
		if(tmp != NULL)
			send(clntSock, &waitting, 4, 0);
		tmp = tmp->next;
	} while (tmp != NULL);
	waitting = -1;
	send(clntSock, &waitting, 4, 0);
}

void startClnt(int clntNo, int clntSock) {
	waitClnt* tmp = createWait(clntNo, clntSock);
	setWait(tmp);
}

//채팅요구 성공하여 채팅방 생성
void startRoom(int clntNo1, int clntNo2) {
	deleteWait(clntNo1);
	deleteWait(clntNo2);
	chatRoom* tmp = createRoom(clntNo1, clntNo2);
	setRoom(tmp);
}

void startFTP(int clntNo) {
	deleteWait(clntNo);
	ftpClnt* tmp = createFTP(clntNo);
	setFTP(tmp);
}

waitClnt* findWait(int clntNo) {
	waitClnt* tmp = head_wait;
	do {
		if (clntNo == tmp->clntNo) {
			return tmp;
		}
		tmp = tmp->next;
	} while (tmp != NULL);
	return NULL;
}

//"/EXIT"을 recv하면 채팅방을 없애고 wait 상태로 만들기
void exitRoom(int clntNo, int clntSock, int partnerSock) {
	int pNum;
	chatRoom* tmp = findRoom(clntNo);
	//이런경우는 없음
	if (tmp == NULL) {}
	//채팅방 찾으면
	else {
		pNum = deleteRoom(tmp) - clntNo;
		//수정 필요해 보임
		startClnt(pNum, partnerSock);
		startClnt(clntNo, clntSock);
	}
}

chatRoom* findRoom(int clntNo) {
	chatRoom* tmp = head_room;
	do {
		int clntNo1 = tmp->clntNo1;
		int clntNo2 = tmp->clntNo2;
		if (clntNo == clntNo1) {
			return tmp;
		}
		if (clntNo == clntNo2) {
			return tmp;
		}
		tmp = tmp->next;
	} while (tmp != NULL);
	return NULL;
}

void exitFTP(int clntNo, int clntSock) {
	deleteFTP(clntNo);
	startClnt(clntNo, clntSock);
}

ftpClnt* findFTP(int clntNo) {
	ftpClnt* tmp = head_ftp;
	do {
		if (clntNo == tmp->clntNo) {
			return tmp;
		}
		tmp = tmp->next;
	} while (tmp != NULL);
	return NULL;
}

void setWait(waitClnt* tmp) {
	if (head_wait == NULL) {
		head_wait = rear_wait = tmp;
	}
	else {
		rear_wait->next = tmp;
		tmp->before = rear_wait;
		rear_wait = tmp;
	}
}

void setFTP(ftpClnt* tmp) {
	if (head_ftp == NULL) {
		head_ftp = rear_ftp = tmp;
	}
	else {
		rear_ftp->next = tmp;
		tmp->before = rear_ftp;
		rear_ftp = tmp;
	}
}

void setRoom(chatRoom* tmp) {
	if (head_room == NULL) {
		head_room = rear_room = tmp;
	}
	else {
		rear_room->next = tmp;
		tmp->before = rear_room;
		rear_room = tmp;
	}
}

int deleteWait(int No) {
	waitClnt* tmp = findWait(No);
	if (tmp->before != NULL)
		tmp->before->next = tmp->next;
	else
		head_wait = tmp->next;

	if (tmp->next != NULL)
		tmp->next->before = tmp->before;
	else
		rear_wait = tmp->before;

	int clntNo = tmp->clntNo;
	free(tmp);
	return clntNo;
}

int deleteFTP(int No) {
	ftpClnt* tmp = findFTP(No);
	if (tmp->before != NULL)
		tmp->before->next = tmp->next;
	else
		head_ftp = tmp->next;

	if (tmp->next != NULL)
		tmp->next->before = tmp->before;
	else
		rear_ftp = tmp->before;

	int clntNo = tmp->clntNo;
	free(tmp);
	return clntNo;
}

int deleteRoom(chatRoom* tmp) {
	int clntNo1 = tmp->clntNo1;
	int clntNo2 = tmp->clntNo2;

	if (tmp->before != NULL)
		tmp->before->next = tmp->next;
	else
		head_room = tmp->next;

	if (tmp->next != NULL)
		tmp->next->before = tmp->before;
	else
		rear_room = tmp->before;

	free(tmp);
	return (clntNo1 + clntNo2);
}

ftpClnt* createFTP(int clntNo) {
	ftpClnt* tmp = (ftpClnt*)malloc(sizeof(ftpClnt));
	tmp->next = NULL;
	tmp->before = NULL;
	tmp->clntNo = clntNo;
	return tmp;
}

waitClnt* createWait(int clntNo, int clntSock) {
	waitClnt* tmp = (waitClnt*)malloc(sizeof(waitClnt));
	tmp->next = NULL;
	tmp->before = NULL;
	tmp->clntNo = clntNo;
	tmp->mySock = clntSock;
	return tmp;
}

chatRoom* createRoom(int clntNo1, int clntNo2) {
	chatRoom* tmp = (chatRoom*)malloc(sizeof(chatRoom));
	tmp->next = NULL;
	tmp->before = NULL;
	tmp->clntNo1 = clntNo1;
	tmp->clntNo2 = clntNo2;
	return tmp;
}