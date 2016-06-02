// FindFiles.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


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
VOID FindFiles(TCHAR *path, 
	         TCHAR *fileMatch,
			 DirectoryProcessor dp, 
			 FileProcessor fp, 
			 LPVOID ctx) {

	HANDLE iterator;
	WIN32_FIND_DATA fileData;
	TCHAR buffer[MAX_PATH];		// auxiliary buffer
	

	// the buffer is needed to define a match string that guarantees 
	// a priori selection for all files
	_stprintf_s(buffer, _T("%s%s"), path, _T("*.*"));

	// start iteration
	if ((iterator = FindFirstFile(buffer, &fileData)) == INVALID_HANDLE_VALUE) 
		return; // iteration failed

	// process directory entries
	do {
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// not processing "." and ".." files!
			if (_tcscmp(fileData.cFileName, _T("."))
				&& _tcscmp(fileData.cFileName, _T(".."))) {
				_stprintf_s(buffer, _T("%s%s/"), path, fileData.cFileName);
				// call DirectoryProcessor callback if there is one
				if (dp != NULL)
					dp(&fileData, path, ctx);
				// recusively process child directory
				FindFiles(buffer, fileMatch, dp, fp, ctx);
			}
		}
		else {
			if (_tstrmatch(fileMatch, fileData.cFileName))  
				// if file match call FileProcessor callback if there is one
				if(fp != NULL)
					fp(&fileData, path, ctx);
		}
		
	} while (FindNextFile(iterator, &fileData));
	FindClose(iterator);
}





