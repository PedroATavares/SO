#include "stdafx.h"
#include "aio.h"
#include "../include/FSCloudService.h"		
#include "../include/serverutils.h"


#define BUF_SIZE 1024

typedef struct upload_ctx {
	LPVOID buffer;
	HANDLE hfile;
	LPDWORD written;
	DWORD toCopy;

} UPLOAD_CTX, *PUPLOAD_CTX;

// retorna a dimensão de um ficheiro com a dimensão máxima de 2GBytes
INT getFileSize(char * fileName) {
	WIN32_FILE_ATTRIBUTE_DATA fData;
	if (!GetFileAttributesExA(fileName, GetFileExInfoStandard, &fData)) return -1;
	return fData.nFileSizeLow;
}

static VOID UploadFileCallBack(PAIO_DEV aiodev, INT transferedBytes, LPVOID ctx) {
	PUPLOAD_CTX context=(PUPLOAD_CTX) ctx ;
	if (context->toCopy > 0) {
		DWORD written;

		if (!WriteFile(context->hfile, context->buffer, transferedBytes, context->written, NULL)) {
			printf("Error writing on file copy\n");
			return;
		}

		context->written+=transferedBytes;
		context->toCopy -= transferedBytes;
		ReadCompleteAsync(aiodev,context->buffer,BUF_SIZE,UploadFileCallBack,ctx);
	}
}

INT uploadFile(SOCKET s, HANDLE fileOut, int toCopy) {
	char buffer[BUF_SIZE];

	printf("copy file with %d bytes!\n", toCopy);
	
	return STATUS_OK;
}

INT downloadFile(HANDLE fileIn, SOCKET s, int toCopy) {
	char buffer[BUF_SIZE];

	
	while (toCopy > 0) {
		DWORD read;

		if (!ReadFile(fileIn, buffer, BUF_SIZE, &read, NULL)) {
			printf("Error reading on downloadFile\n");
			return IO_ERROR;
		}
		if (!writeFromBuffer(s, buffer, read))  {
			printf("Error writing on socket\n");
			return IO_ERROR;
		}
		toCopy -= read;
	}
	return STATUS_OK;
}

INT downloadFile(char* fileInName, SOCKET  s, int toCopy) {
	HANDLE hFile = CreateFileA(fileInName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return FS_ERROR;
	}
	int status = downloadFile(hFile, s, toCopy);
	CloseHandle(hFile);
	return status;
}

BOOL readToBuffer(SOCKET dev, LPVOID buffer, DWORD toRead) { 
	char *buf = (char *)buffer;
	DWORD transferedBytes, totalRead=0;
	
	while (toRead > 0)
	{
		if ((transferedBytes= recv(dev, buf+totalRead, toRead, 0)) <= 0)
			// it means that connection was closed by peer or there was a commm error
			return FALSE;

		toRead -= transferedBytes;
		totalRead += transferedBytes;
	}

	return TRUE;
}

BOOL writeFromBuffer(SOCKET dev, LPVOID buffer, DWORD toWrite)
{
	DWORD transferedBytes = 0, totalWrite = 0;
	char *buf = (char *)buffer;
	 
	while (toWrite > 0)
	{
		if ((transferedBytes = send(dev, buf + totalWrite, toWrite, 0))  <= 0)
			// it means that connection was closed by peer or there was a commm error
			return FALSE;
		toWrite -= transferedBytes;
		totalWrite += transferedBytes;
	}
	 
	return TRUE;
}