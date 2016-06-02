// src.cpp : Defines the entry point for the console application.
//
// Checking the allocation in two separated phases: reserve, then committ
//
// Check system change with task manager and resource monitor.
//
// JM, 2015
//

#include "stdafx.h"

#define INTERACTIVE FALSE

// 512MB allocation!
#define MEM_SIZE (1024*1024*512)

void Delay() {
	Sleep(1000);
}


void Next(BOOL interactive) {
	if (interactive) {
		_tprintf(_T("Press return to continue..."));
		getchar();
	}
	else {
		Delay();
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	
	SYSTEM_INFO inf;
	
	_tprintf(_T("Init!\n"));
	Next(TRUE);

	HMODULE base = GetModuleHandle(NULL);
	_tprintf(_T("program loaded at=%p\n"), (LPVOID) base);
	// Just show the minimum reserve size (granularity size) end page size
	GetSystemInfo(&inf);
	_tprintf(_T("minimum region size=%X\n"), inf.dwAllocationGranularity);
	_tprintf(_T("page size=%X\n"), inf.dwPageSize);
	
	
	Next(INTERACTIVE);

	_tprintf(_T("Reserve region with %X bytes!\n"), MEM_SIZE);
	LPBYTE buf1 = (LPBYTE) VirtualAlloc(NULL, MEM_SIZE, MEM_RESERVE, 
		PAGE_READWRITE);
	if (buf1 == NULL) {
		_tprintf(_T("Error %d reserving region!\n"), GetLastError());
		return 0;
	}
	_tprintf(_T("Region reserved at %p\n"), buf1);

	Next(INTERACTIVE);

	LPBYTE buf2 = (LPBYTE) VirtualAlloc(buf1+MEM_SIZE/2, MEM_SIZE/2, MEM_COMMIT, 
		PAGE_READWRITE);
	 

	_tprintf(_T("buf2=%p\n"), buf2);
	_tprintf(_T("Commited!\n"));
	
	Next(INTERACTIVE);

	for(DWORD i=0; i < MEM_SIZE/(inf.dwPageSize*2) ; ++i)
		buf2[i*inf.dwPageSize] = (BYTE) i;
	
	_tprintf(_T("Changed!\n"));
	
	Next(INTERACTIVE);

	VirtualFree(buf1, 0, MEM_DECOMMIT);
	_tprintf(_T("Decommited!\n"));
	
	Next(INTERACTIVE);

	VirtualFree(buf1, 0, MEM_RELEASE);
	_tprintf(_T("Released!\n"));
	Next(INTERACTIVE);

	_tprintf(_T("End!\n"));
	 
	return 0;
}


