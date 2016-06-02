#include<Windows.h>
#include <stdio.h>
#include <psapi.h>

static DWORD getPageSize() {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwPageSize;
}

void MemoryLeak(int id, int threshold){
	DWORD code;
	int count = 0;
	DWORD size= sizeof(PSAPI_WORKING_SET_BLOCK);
	PPSAPI_WORKING_SET_INFORMATION psapi = (PPSAPI_WORKING_SET_INFORMATION)malloc(sizeof(PSAPI_WORKING_SET_BLOCK));
	DWORD timeToSleep=5000;
	DWORD maxSize=100;		//Max 50 char procName
	
	printf("Id=%d, Threshold=%d\n", id,threshold);
	HANDLE hprocess =OpenProcess(PROCESS_ALL_ACCESS, TRUE, id);
	if(!hprocess){
		printf("Null Handle: Could Not Find Process");
		return;
	}
	
	LPTSTR procName = (LPTSTR)malloc(maxSize);
	GetModuleFileNameEx(hprocess,NULL,procName,maxSize);
	printf("Process Name= %s\n", procName);
	GetExitCodeProcess(hprocess,&code);
	while (code == STILL_ACTIVE){
		while (!QueryWorkingSet(hprocess, psapi, size)) {
			size = psapi->NumberOfEntries * sizeof(PSAPI_WORKING_SET_BLOCK) + 100;
			psapi = (PPSAPI_WORKING_SET_INFORMATION)realloc(psapi, size);
		}
		
		
		for (int i = 0; i < psapi->NumberOfEntries; i++) {
			if(!psapi->WorkingSetInfo[i].Shared)
				count ++;
		}
		int privateSet = count * getPageSize() / 1024;
		if(privateSet > threshold)
			printf("Program surpassed the threshold, leaking -> Used (KB): %d\n", privateSet);
		else 
			printf("%d Private Used (KB)\n", privateSet);
		Sleep(timeToSleep);
		count=0;
		GetExitCodeProcess(hprocess,&code);
	}
	free(psapi);
	CloseHandle(hprocess);
	
}


void main(DWORD argc, PCHAR argv[]){
	if(argc<2){
		printf("Not enough arguments");	
		return;
	}
	PCHAR end;
	long processID = strtol(argv[1], &end, 10);
	long threshold = strtol(argv[2], &end, 10);
	MemoryLeak(processID, threshold);
	system("Pause");
}