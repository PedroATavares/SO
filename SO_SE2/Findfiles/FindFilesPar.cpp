// FindFiles.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "findfilespar.h"

#define N_CPU 1;
const DWORD maxThreads = N_CPU;
DWORD currThreads = 0;

/*-----------------------------------------------------------------------
This function allows the processing of a selected set of files in a directory tree
It uses the Windows functions for directory file iteration, namely
"FindFirstFile" and "FindNextFile"

Parameters:
	path - the pathname of the root directory to process
	fileMatch - a pattern (ex: *.pdf) for file selection
	dp - a callback called every time a chils directory starts processing
	fp - a callback called every time a find is selected for processing
	ctx- an arbitrary user context passed to "dp" and "fp"
--------------------------------------------------------------------------*/
VOID FindFilesPar(LPVOID parameters) {
	
	Parameters param=*(PParameters)parameters;
	HANDLE iterator;
	WIN32_FIND_DATA fileData;
	TCHAR buffer[MAX_PATH];		// auxiliary buffer
	HANDLE threads[maxThreads];
	DWORD i=0;
	//_tprintf_s(_T("Thread in: %s\n"), param.path);
	// the buffer is needed to define a match string that guarantees 
	// a priori selection for all files
	_stprintf_s(buffer, _T("%s%s"), param.path, _T("*.*"));
	
	// start iteration
	if ((iterator = FindFirstFile(buffer, &fileData)) == INVALID_HANDLE_VALUE) 
		return; // iteration failed

	// process directory entries
	do {
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// not processing "." and ".." files!
			if (_tcscmp(fileData.cFileName, _T("."))
				&& _tcscmp(fileData.cFileName, _T(".."))) {
				_stprintf_s(buffer, _T("%s%s/"), param.path, fileData.cFileName);
				// call DirectoryProcessor callback if there is one
				if (param.dp != NULL)
					param.dp(&fileData, param.path, param.ctx);
				// recusively process child directory
				Parameters aux = param;
				aux.path = buffer;
				if (currThreads != maxThreads) {
					currThreads++;
					threads[i++]=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FindFilesPar, &aux , 0, NULL);
				}else{
					WaitForMultipleObjects(i, threads, FALSE, INFINITE);
					threads[i++] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FindFilesPar, &aux, 0, NULL);
				}
			}
		}
		else {
			if (_tstrmatch(param.fileMatch, fileData.cFileName))  
				// if file match call FileProcessor callback if there is one
				if(param.fp != NULL)
					param.fp(&fileData, param.path, param.ctx);
		}
		
	} while (FindNextFile(iterator, &fileData));
	WaitForMultipleObjects(i,threads,TRUE,INFINITE);
//	_tprintf_s(_T("Thread Finished in: %s\n"), param.path);
	currThreads--;
//	for(int k=0;k>i;k++) CloseHandle(threads[k]);
	FindClose(iterator);
}