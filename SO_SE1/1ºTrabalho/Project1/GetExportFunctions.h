#include <Windows.h>
#include <stdio.h>
#include <string.h>


//DWORD GetExportFunctions(LPCTSTR dllfilepath, ExpFuncs * namesOut);


typedef struct { DWORD len; TCHAR * names[1]; } ExpFuncs;



