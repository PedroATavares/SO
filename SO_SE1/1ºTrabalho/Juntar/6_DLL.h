#include <Windows.h>
#include <stdio.h>
#include <string.h>


#ifdef GETEXPORTFUNCLIBRARY_EXPORTS
#define GETEXPORTFUNCLIBRARY_EXPORTS __declspec(dllexport) 
#else
#define GETEXPORTFUNCLIBRARY_EXPORTS __declspec(dllimport) 
#endif

typedef struct { DWORD len; TCHAR * names[1]; } ExpFuncs;

GETEXPORTFUNCLIBRARY_EXPORTS DWORD GetExportFunctions(LPCTSTR dllfilepath, ExpFuncs * namesOut);
