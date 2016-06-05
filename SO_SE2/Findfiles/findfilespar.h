#pragma once

VOID FindFilesPar(LPVOID parameters);

typedef struct {
	TCHAR *path;
	TCHAR *fileMatch;
	DirectoryProcessor dp;
	FileProcessor fp;
	LPVOID ctx;
} Parameters, *PParameters;
