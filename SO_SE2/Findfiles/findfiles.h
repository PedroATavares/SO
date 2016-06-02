#pragma once

typedef VOID(*FileProcessor)(LPWIN32_FIND_DATA fileData, TCHAR *path, VOID *ctx);
typedef VOID(*DirectoryProcessor)(LPWIN32_FIND_DATA fileData, TCHAR *path, VOID *ctx);

VOID FindFiles(TCHAR *path,
	TCHAR *fileMatch,
	DirectoryProcessor dp,
	FileProcessor fp,
	LPVOID ctx);