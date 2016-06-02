#pragma once

#ifndef UTMULJOIN_DLL
#define UTMULJOIN_API __declspec(dllimport)
#else
#define UTMULJOIN_API __declspec(dllexport)
#endif

#include <Windows.h>


#ifdef __cplusplus
extern "C" {
#endif

UTMULJOIN_API
BOOL UtMultJoin(HANDLE handle[], int size);

#ifdef __cplusplus
} // extern "C"
#endif
