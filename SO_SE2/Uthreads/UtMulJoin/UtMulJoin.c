
#include <crtdbg.h>
#include "UThreadInternal.h"


#include "UtMulJoin.h"
#include "USynch.h"
#include "UThread.h"
#include "List.h"
#include "UThreadInternal.h"

BOOL UtMultJoin(HANDLE handle[], int size) {

	PUTHREAD thread = (PUTHREAD)UtSelf();
	UtInitCounterLatch(&thread->cLatch, size);

	int i = 0;
	for (;i < size;i++) {
		if (handle[i] == UtSelf() || !UtAlive(handle[i]))
			return FALSE;
	}

	for (i = 0;i < size;++i) {
		((PUTHREAD)handle[i])->NRelease++;
		InsertHeadList(&((PUTHREAD)handle[i])->waitingLink, &thread->cLatchLink);
	}

	UtDeactivate();
	//UtWaitCounterLatch(&thread->cLatch);
	return TRUE;
}