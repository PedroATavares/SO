#pragma once

VOID FindFilesPar(TCHAR *path, TCHAR *fileMatch,
	DirectoryProcessor dp, FileProcessor fp, LPVOID ctx);

VOID FindFilesParAux(LPVOID parameters);

typedef struct {
	TCHAR				*path;
	TCHAR				*fileMatch;
	DirectoryProcessor	dp;
	FileProcessor		fp;
	LPVOID				ctx;
	HANDLE				semaphore;
	BOOL				recursive;
} Parameters, *PParameters;
