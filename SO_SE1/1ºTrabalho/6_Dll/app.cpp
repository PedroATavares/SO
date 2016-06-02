 #include <windows.h>
#include <stdio.h>
#include "6_DLL.h"

DWORD main(DWORD argc, PCHAR argv[]) {
	LPSTR name = "C:\\Windows\\System32\\kernel32.dll";
	//DumpFile(name);
	DWORD k= GetExportFunctions(name, NULL);
	ExpFuncs* func=(ExpFuncs*)malloc(k+4);
	GetExportFunctions(name,func);
	printf("%d",func->len);
	system("PAUSE");
	return 0;
}