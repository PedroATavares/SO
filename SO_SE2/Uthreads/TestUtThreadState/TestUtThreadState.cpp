// SimpleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "usynch.h"

/////////////////////////////////////////////
//
// CCISEL 
// 2007-2010
//
// UThread    Library First    Test
//
// Jorge Martins, 2011
////////////////////////////////////////////
#define DEBUG

#define MAX_THREADS 5

#include <crtdbg.h>
#include <stdio.h>

#include "..\Include\Uthread.h"
#include "..\Include\USynch.h"
#include "UtMulJoin.h"

///////////////////////////////////////////////////////////////
//															 //
// Test 1: N threads, each one printing its number M times //
//															 //
///////////////////////////////////////////////////////////////

ULONG Test1_Count;
HANDLE ut[MAX_THREADS];
DWORD id=1;

VOID Test1_Thread(UT_ARGUMENT Argument) {
	UCHAR Char;
	ULONG Index;
	Char = (UCHAR)Argument;

	for (Index = 0; Index < 10; ++Index) {
		printf("Thread %c \n",Char);
		printf("%d = %d (Running) \n", UtThreadState(UtSelf()), Running);
		if ((rand() % 4) == 0) {
			UtYield();
		}
	}

	++Test1_Count;

}

VOID Test2_Thread(UT_ARGUMENT Argument) {
	UCHAR Char;
	ULONG Index;
	Char = (UCHAR)Argument;

	for (Index = 0; Index < 10; ++Index) {
		printf("Thread %c \n", Char);
		printf("%d = %d (Running) \n", UtThreadState(UtSelf()), Running);
		if ((rand() % 4) == 0) {
			UtYield();
			UtTerminateThread(UtSelf());
		}
	}

	++Test1_Count;

}

VOID Test3_Thread(UT_ARGUMENT Argument) {
	UCHAR Char;
	ULONG Index;
	Char = (UCHAR)Argument;
	UtMultJoin(ut, id++ -1);
	for (Index = 0; Index < 10; ++Index) {
		printf("Thread %c \n", Char);
		printf("%d = %d (Running) \n", UtThreadState(UtSelf()), Running);
		if ((rand() % 4) == 0) {
			UtYield();
			
		}
	}

	++Test1_Count;

}

VOID TestUtAliveAndUtState() {
	ULONG Index;
	HANDLE ut;

	Test1_Count = 0;

	printf("\n :: Test 1 - BEGIN :: \n\n");

	for (Index = 0; Index < MAX_THREADS; ++Index) {
		ut= UtCreate(Test1_Thread, (UT_ARGUMENT)('0' + Index));
		printf("%d = %d (Ready) \n", UtThreadState(ut), Ready);
		printf("UThread %d Alive? : %d \n",'0'+Index, UtAlive(ut));
	}

	UtRun();
	printf("-------After UtRun()------- \n");
	printf("UThread %d Alive? : %d \n", '0' + --Index, UtAlive(ut));

	_ASSERTE(Test1_Count == MAX_THREADS);
	printf("\n\n :: Test 1 - END :: \n");
}


VOID TestUtTerminate() {
	ULONG Index;
	HANDLE ut;

	Test1_Count = 0;

	printf("\n :: Test 2 - BEGIN :: \n\n");

	for (Index = 0; Index < 5; ++Index) {
		ut = UtCreate(Test2_Thread, (UT_ARGUMENT)('0' + Index));
		printf("%d = %d (Ready) \n", UtThreadState(ut), Ready);
		printf("UThread %d Alive? : %d \n", '0' + Index, UtAlive(ut));
	}

	UtRun();
	printf("-------After UtRun()------- \n");
	printf("UThread %d Alive? : %d \n", '0' + --Index, UtAlive(ut));


	printf("\n\n :: Test 2 - END :: \n");
}

VOID TestUtMulJoin() {
	ULONG Index;
	const DWORD  NThreads=5;
	Test1_Count = 0;
	printf("\n :: Test 3 - BEGIN :: \n\n");

	for (Index = 0; Index < NThreads; ++Index) {
		ut[Index]= UtCreate(Test3_Thread, (UT_ARGUMENT)('0' + Index));
		printf("%d = %d (Ready) \n", UtThreadState(ut), Ready);
		printf("UThread %d Alive? : %d \n", '0' + Index, UtAlive(ut));
	}

	UtRun();
	printf("-------After UtRun()------- \n");
	printf("UThread %d Alive? : %d \n", '0' + --Index, UtAlive(ut));


	printf("\n\n :: Test 3 - END :: \n");
}


int main() {
	UtInit();

	//TestUtAliveAndUtState();
	//TestUtTerminate();
	TestUtMulJoin();
	printf("Press any key to finish");
	getchar();


	UtEnd();
	return 0;
}


