#pragma once


// é usado o nível de concorrencia por omissao
#define MAX_CONCURRENCY 0

// threads associadas à completion port
#define MAX_THREADS	16

struct _aio_dev;

// callback signature
typedef VOID(*AIO_CALLBACK)(struct _aio_dev *dev,  INT transferedBytes, LPVOID ctx);

// oper context and callback
typedef struct _aio_oper {
	AIO_CALLBACK callback;
	LPVOID ctx;
} AIO_OPER, *PAIO_OPER;

// Represents an async device (file or socket).
// The model only supports sequential access and
// at most one operation in course per device.
typedef struct _aio_dev {
	HANDLE dev;
	OVERLAPPED ioStatus;
	AIO_OPER oper;
	BOOL isSocket;
} AIO_DEV, *PAIO_DEV;


/* completion port public functions */
BOOL CreateCompletionPort(int maxConcurrency);
BOOL CompletionPortAssociateHandle(HANDLE devHandle, LPVOID completionKey);

/* low level async I/O wrappers */
BOOL AsyncRead(HANDLE sd, LPVOID buffer, DWORD length, OVERLAPPED *ovr);
BOOL AsyncWrite(HANDLE sd, LPVOID buffer, DWORD length, OVERLAPPED *ovr);


/* aio devices creation and destruction */
PAIO_DEV NewAioSocket(SOCKET s);
PAIO_DEV CreateFileAsync(LPCSTR fileName);
PAIO_DEV OpenFileAsync(LPCSTR fileName);
VOID CloseAsync(PAIO_DEV ad);

/* aiodev management */
VOID SetAioOper(PAIO_DEV dev, AIO_CALLBACK cb, LPVOID uCtx);
VOID ProcessTransfer(PAIO_DEV dev, DWORD transferedBytes);


// async operations
BOOL ReadAsync(PAIO_DEV dev, LPVOID buffer, int bsize, AIO_CALLBACK cb, LPVOID ctx);
BOOL WriteAsync(PAIO_DEV dev, LPVOID buffer, int bsize, AIO_CALLBACK cb, LPVOID ctx); 

// not existing yet operations
BOOL WriteCompleteAsync(PAIO_DEV dev, LPVOID buffer, int bsize, AIO_CALLBACK cb, LPVOID ctx);
BOOL ReadCompleteAsync(PAIO_DEV dev, LPVOID buffer, int bsize, AIO_CALLBACK cb, LPVOID ctx);
BOOL CopyStreamAsync(PAIO_DEV devIn, PAIO_DEV devOut, int size, AIO_CALLBACK cb, LPVOID ctx);

/* start socket service */
VOID StartSession(SOCKET s);
