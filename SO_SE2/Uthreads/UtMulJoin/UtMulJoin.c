
#include <crtdbg.h>
#include "UThreadInternal.h"


#include "UtMulJoin.h"
#include "USynch.h"
#include "UThread.h"
#include "List.h"
#include "UThreadInternal.h"

BOOL UtMultJoin(HANDLE handle[], int size) {

	if (size<1) return FALSE;

	PUTHREAD thread = (PUTHREAD)UtSelf();

	int i = 0;
	for (;i < size;i++) {
		if (handle[i] == UtSelf() || !UtAlive(handle[i]))
			return FALSE;
	}
	thread->NWaiting=size;
	for (i = 0;i < size;++i) {
		((PUTHREAD)handle[i])->NRelease++;
		InsertTailList(&((PUTHREAD)handle[i])->waitingLink, &thread->cLatchLink);
	}

	UtDeactivate();
	return TRUE;
}