#include "stdafx.h"
#include "findfilespar.h"
/*----------------------------------------------------------------------
This Test counts the selected files.
Selected file are:

1-) just the files with names that  match the specified pattern
or

2) if an adittional word is given as a program argument, selected  files
are the file that match the specified pattern and the word ocourrs in contents

------------------------------------------------------------------------*/


// serves as user processing context
typedef struct MyContext {
	LONG countFiles;		// count all selected files
	CHAR *match;			// pattern used a name filter
	LONG countDirs;			// count all traversed directories
	FILE* logFile;			// file used to log names of processed directories 
} MyContext;


/*
	Counts the selected file if the contents have the user specified word.
	The file name is listed in standard output
*/
VOID WordInFile(LPWIN32_FIND_DATA fileData, TCHAR *path, VOID *ctx)
{
	MyContext *stats = (MyContext*) ctx;
	TCHAR buffer[MAX_PATH];
	CHAR line[256];

	_stprintf_s(buffer, _T("%s%s"), path, fileData->cFileName);
	FILE *file;
	int error = _tfopen_s(&file, buffer, _T("r"));
	if (error != 0) return;
	while (fgets(line, 256, file) != NULL)
	{
		if (strstr(line, stats->match) != NULL)
		{
			stats->countFiles++;
			// show the name in standard output
			_tprintf(_T("  %s%s\n"), path, fileData->cFileName);
			break;
		}
	}
	fclose(file);
}

/*
 counts the file with a name that matches de specified pattern
 and shows the name in standard output
*/
VOID CountFile(LPWIN32_FIND_DATA fileData, TCHAR *path, VOID *ctx) {
	MyContext *stats = (MyContext*) ctx;
	stats->countFiles++;
	_tprintf_s(_T("%s%s\n"), path, fileData->cFileName);
}

/*
 Count and log the traversed dirs
*/
VOID CountDir(LPWIN32_FIND_DATA fileData, TCHAR *path, VOID *ctx) {
	MyContext *stats = (MyContext*) ctx;
	
	// log and count all directories
	_ftprintf(stats->logFile, _T("%s%s\n"), path, fileData->cFileName);
	stats->countDirs++;
}


int _tmain(int argc, _TCHAR* argv[])
{
	MyContext ctx = { 0 };
	FILE *file;
	FileProcessor fp;
#define MAX_MATCH_SIZE 128
	char match[MAX_MATCH_SIZE];   

	if (argc < 3 || argc > 4)
	{
		printf("usage: findfiles <root dir> <file pattern> [ <word in file> ]\n");
		return 1;
	}
	if (argv[1][_tcslen(argv[1]) - 1] != '/')
	{
		printf("root dir name must end with a slash ('/')\n");
		return 1;
	}
	// create file for traversed directories log
	int error = _tfopen_s(&file, _T("Log.txt"), _T("w"));
	if (error != 0)
	{
		printf("Error creating log file!\n");
		return 2;
	}
	
	
	// set logFile in context
	ctx.logFile = file;

	// initialize adittional context
	if (argc == 4) {
		// just use text files in ascii
		w2c_strcpy(match, argv[3]);
		ctx.match = match;
		// if word is specified use the file processor to match a word in a file
		fp = WordInFile;
	}
	else
	{
		// if word is not specified use the file processor to just count the file
		fp = CountFile;
	}

	DWORD startTime = GetTickCount();

	FindFilesPar(argv[1],argv[2],CountDir,fp,&ctx);
	DWORD endTime = GetTickCount();

	printf("\nDone in %d ms!\n", endTime - startTime);
	printf("Total Files: %d\n", ctx.countFiles);
	printf("Total Dirs: %d\n", ctx.countDirs);
	printf("Press any key to finish");
	getchar();
	
	 
	return 0;
}

