#include "GetExportFunctions.h"


static PVOID RVAToPointer(PVOID base, DWORD rva, PIMAGE_FILE_HEADER pifh, PIMAGE_SECTION_HEADER pfsh) {

	PIMAGE_SECTION_HEADER psh = NULL;

	for (int i = 0; i < pifh->NumberOfSections; ++i) {

		if (pfsh[i].VirtualAddress <= rva && rva < pfsh[i].VirtualAddress + pfsh[i].SizeOfRawData) {
			
			psh = &pfsh[i];
			break;
		}
	}

	if (psh == NULL) {
		printf("No Section Header");
	}
	else {
		return (PVOID)((DWORD)base + rva - (psh->VirtualAddress - psh->PointerToRawData));
	}

}

static LPSTR checkErr() {
	DWORD errorMessageID = GetLastError();
	if (errorMessageID == 0)
		return NULL;

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorMessageID,
		MAKELANGID(
			LANG_NEUTRAL,
			SUBLANG_DEFAULT
			),
		(LPSTR)&messageBuffer,
		0,
		NULL
		);
	messageBuffer = (LPSTR)malloc(size);
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorMessageID,
		MAKELANGID(
			LANG_NEUTRAL,
			SUBLANG_DEFAULT
			),
		(LPSTR)&messageBuffer,
		size,
		NULL
		);

	return messageBuffer;
}


DWORD GetExportFunctions(LPCTSTR dllfilepath, ExpFuncs * namesOut) {

	HANDLE hFile;
	HANDLE hFileMapping;
	PBYTE lpFileBase;
	PIMAGE_DOS_HEADER pidh;

	hFile = CreateFile(dllfilepath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf(checkErr());
		printf("Couldn't open file with CreateFile()\n");
		return 0;
	}

	hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hFileMapping == NULL)
	{
		CloseHandle(hFile);
		printf("Couldn't open file mapping with CreateFileMapping()\n");
		return 0;
	}

	lpFileBase = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
	if (lpFileBase == NULL)
	{
		CloseHandle(hFileMapping);
		CloseHandle(hFile);
		printf("Couldn't map view of file with MapViewOfFile()\n");
		return 0;
	}

	printf("Dump of file %s\n\n", dllfilepath);

	pidh = (PIMAGE_DOS_HEADER)lpFileBase;
	if (pidh->e_magic == IMAGE_DOS_SIGNATURE)
	{

		PIMAGE_NT_HEADERS32 pinh = (PIMAGE_NT_HEADERS32)((PBYTE)pidh + pidh->e_lfanew);
		PIMAGE_FILE_HEADER pifh = (PIMAGE_FILE_HEADER)&pinh->FileHeader;
		PIMAGE_OPTIONAL_HEADER pioh = (PIMAGE_OPTIONAL_HEADER)&pinh->OptionalHeader;

		PIMAGE_SECTION_HEADER pfsh = (PIMAGE_SECTION_HEADER)((PBYTE)pioh + pifh->SizeOfOptionalHeader);

		DWORD vae = (DWORD)(pioh->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);



		IMAGE_EXPORT_DIRECTORY * exports = (IMAGE_EXPORT_DIRECTORY*)RVAToPointer(lpFileBase, vae, pifh, pfsh);

		vae = exports->AddressOfNames;

		PCHAR * names = (PCHAR *)RVAToPointer(lpFileBase, vae, pifh, pfsh);

		DWORD size=0;

		for (int i = 0;i<exports->NumberOfNames;i++) {
			size+=lstrlen((LPCSTR)RVAToPointer(lpFileBase, (DWORD)names[i], pifh, pfsh))+1;
		}

		if (namesOut==NULL||namesOut->len<size) {
			return size;
		}

		namesOut->len = 0;

		for (int i = 0;i<exports->NumberOfNames;i++) {
			LPTSTR aux= (LPTSTR)RVAToPointer(lpFileBase, (DWORD)names[i], pifh, pfsh);
			lstrcpy((LPTSTR)namesOut->names+namesOut->len,aux);
			namesOut->len += lstrlen(aux) + 1;
		}

	}
	else {
		printf("unrecognized file format\n");
	}
	UnmapViewOfFile(lpFileBase);
	CloseHandle(hFileMapping);
	CloseHandle(hFile);
	return namesOut->len;
}