#pragma once

#define SERVER_PORT 5002

#define MAX_CMD_NAME    8
#define MAX_FILE_NAME 128
#define MAX_SIZE_CHARS 16

#define MAX_STATUS_CHARS 4

// commands
#define PUT_FILE	"put"
#define GET_FILE	"get"
#define LIST_FILES	"list"

// errors
#define STATUS_OK		0
#define INTERNAL_ERROR	1
#define IO_ERROR		2
#define FS_ERROR		3

typedef struct _fsCloudRequestHeader {
	char cmdName[MAX_CMD_NAME];
	char fileName[MAX_FILE_NAME];
	char fileSize[MAX_SIZE_CHARS];
} FSCloudRequestHeader, *PFSCloudRequestHeader;

typedef struct _fsCloudAnswer {
	char status  [MAX_STATUS_CHARS];
	char dataSize[MAX_SIZE_CHARS];
} FSCloudAnswer, *PFSCloudAnswer;

typedef struct _fsCloudFileEntry {
	char name[MAX_FILE_NAME];
	char size[MAX_SIZE_CHARS];
} FSCloudFileEntry, *PFSCloudFileEntry;