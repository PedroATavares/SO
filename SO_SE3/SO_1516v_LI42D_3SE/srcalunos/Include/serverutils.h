#pragma once

INT getFileSize(char* fileName);
INT uploadFile(SOCKET s, HANDLE fileOut, int toCopy);
INT downloadFile(HANDLE fileIn, SOCKET s, int toCopy);
INT downloadFile(char* fileInName, SOCKET  s, int toCopy);

BOOL readToBuffer(SOCKET sd, LPVOID buffer, DWORD toRead);
BOOL writeFromBuffer(SOCKET sd, LPVOID buffer, DWORD toWrite);
