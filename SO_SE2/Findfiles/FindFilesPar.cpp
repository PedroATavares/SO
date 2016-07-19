// FindFiles.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "findfilespar.h"

#define N_CPU 4;
const DWORD maxThreads = 4*N_CPU;

VOID FindFilesPar(TCHAR *path, TCHAR *fileMatch,
	DirectoryProcessor dp, FileProcessor fp, LPVOID ctx) {
	Parameters param;
	param.path = path;
	param.fileMatch = fileMatch;
	param.dp = dp;
	param.fp = fp;
	param.ctx = ctx;
	param.recursive=FALSE;
	HANDLE semaphore=OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,(LPCWSTR)"Thread_Semaphore");
	if(semaphore==NULL)
		semaphore=CreateSemaphore(NULL,maxThreads,maxThreads, (LPCWSTR)"Thread_Semaphore");
	param.semaphore=semaphore;

	FindFilesParAux(&param);
	CloseHandle(param.semaphore);
};


VOID FindFilesParAux(LPVOID parameters) {
	
	PParameters param= (PParameters)malloc(sizeof(Parameters));
	memcpy(param,parameters,sizeof(Parameters));

	HANDLE iterator;
	WIN32_FIND_DATA fileData;
	TCHAR buffer[MAX_PATH];		// auxiliary buffer
	HANDLE threads[maxThreads];
	DWORD i = 0;


//	_tprintf_s(_T("Thread in: %s\n"), param.path);
	// the buffer is needed to define a match string that guarantees 
	// a priori selection for all files
	
	_stprintf_s(buffer, _T("%s%s"), param->path, _T("*.*"));
	

	// start iteration
	if ((iterator = FindFirstFile(buffer, &fileData)) == INVALID_HANDLE_VALUE) 
		return; // iteration failed

	// process directory entries
	do {
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

			// not processing "." and ".." files!
			if (_tcscmp(fileData.cFileName, _T("."))
				&& _tcscmp(fileData.cFileName, _T(".."))) {


				_stprintf_s(buffer, _T("%s%s/"), param->path, fileData.cFileName);
			
				
				
				// call DirectoryProcessor callback if there is one
				if (param->dp != NULL)
					param->dp(&fileData, param->path, param->ctx);
				
				// recusively process child directory
				
				PParameters aux =(PParameters) malloc(sizeof(Parameters));
				memcpy(aux, param, sizeof(Parameters));

				aux->path=(TCHAR*)malloc(sizeof(TCHAR)*sizeof(buffer) + 1);
				memcpy(aux->path, buffer, sizeof(TCHAR)*sizeof(buffer));

				if(WaitForSingleObject(aux->semaphore,100)!=WAIT_TIMEOUT){
					aux->recursive=FALSE;
					threads[i++]=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FindFilesParAux, aux , 0, NULL);
				}else{
					aux->recursive=TRUE;
					FindFilesParAux(aux);
				}
			}
			
		}
		else {
			if (_tstrmatch(param->fileMatch, fileData.cFileName))  
				// if file match call FileProcessor callback if there is one
				if(param->fp != NULL)
					param->fp(&fileData, param->path, param->ctx);
		}

	} while (FindNextFile(iterator, &fileData));

	WaitForMultipleObjects(i,threads,TRUE,INFINITE);
	
	for(int k=0;k>i;k++) CloseHandle(threads[k]);

	FindClose(iterator);
	if(!param->recursive)
		ReleaseSemaphore(param->semaphore, 1, NULL);
}