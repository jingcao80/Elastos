
#ifndef _ELASTOS_CORELIBRARY_ATOMIC_H__
#define _ELASTOS_CORELIBRARY_ATOMIC_H__

#include "coredef.h"
#include <cutils/atomic.h>          /* use common Android atomic ops */
#include <cutils/atomic-inline.h>   /* and some uncommon ones */

/*
 * If the value at "addr" is equal to "oldvalue", replace it with "newvalue"
 * and return 0.  Otherwise, don't swap, and return nonzero.
 */
int QuasiAtomicCas64(int64_t oldvalue, int64_t newvalue,
        volatile int64_t* addr);

#endif //_ELASTOS_CORELIBRARY_ATOMIC_H__
