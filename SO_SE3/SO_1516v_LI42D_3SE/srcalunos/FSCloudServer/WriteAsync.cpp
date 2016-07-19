#include "stdafx.h"
#include "aio.h"

BOOL WriteAsync(PAIO_DEV dev, LPVOID buffer, int bsize, AIO_CALLBACK cb, LPVOID ctx) {
	SetAioOper(dev, cb, ctx);

	return AsyncWrite(dev->dev, buffer, bsize, &dev->ioStatus);
}