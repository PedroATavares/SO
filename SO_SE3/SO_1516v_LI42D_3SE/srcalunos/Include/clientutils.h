#pragma once

INT getFileSize(char* fileName);
INT downloadFile(SOCKET s, CHAR* fileOut, int toCopy);
INT uploadFile(HANDLE fileIn, SOCKET s, int toCopy);
INT uploadFile(char* fileInName, SOCKET  s, int toCopy);

BOOL readToBuffer(SOCKET sd, LPVOID buffer, DWORD toRead);
BOOL writeFromBuffer(SOCKET sd, LPVOID buffer, DWORD toWrite);
