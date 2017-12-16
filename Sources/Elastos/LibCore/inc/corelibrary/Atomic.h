//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef _ELASTOS_CORELIBRARY_ATOMIC_H__
#define _ELASTOS_CORELIBRARY_ATOMIC_H__

#include "coredef.h"
#include <cutils/atomic.h>          /* use common Android atomic ops */

/*
 * If the value at "addr" is equal to "oldvalue", replace it with "newvalue"
 * and return 0.  Otherwise, don't swap, and return nonzero.
 */
int QuasiAtomicCas64(int64_t oldvalue, int64_t newvalue,
        volatile int64_t* addr);

#endif //_ELASTOS_CORELIBRARY_ATOMIC_H__
