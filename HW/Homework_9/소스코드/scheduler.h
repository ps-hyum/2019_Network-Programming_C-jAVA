typedef struct chatroom {
	int clntNo1;
	int clntNo2;
	struct chatroom* before;
	struct chatroom* next;
}chatRoom;

typedef struct waitclnt {
	int clntNo;
	struct waitclnt* before;
	struct waitclnt* next;
	int mySock;
}waitClnt;

typedef struct ftpclnt {
	int clntNo;
	struct ftpclnt* before;
	struct ftpclnt* next;
}ftpClnt;

chatRoom* head_room;
chatRoom* rear_room;
waitClnt* head_wait;
waitClnt* rear_wait;
ftpClnt* head_ftp;
ftpClnt* rear_ftp;
int clntID;