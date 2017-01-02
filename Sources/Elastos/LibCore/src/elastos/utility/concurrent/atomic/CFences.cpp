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

#include "CFences.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

Int32 CFences::sTheVolatile = 0;

CAR_INTERFACE_IMPL(CFences, Singleton, IFences)

CAR_SINGLETON_IMPL(CFences)

ECode CFences::OrderReads(
    /* [in] */ IInterface* ref,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    Int32 UNUSED(ignore) = sTheVolatile;
    *outface = ref;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CFences::OrderWrites(
    /* [in] */ IInterface* ref,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    sTheVolatile = 0;
    *outface = ref;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CFences::OrderAccesses(
    /* [in] */ IInterface* ref,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    sTheVolatile = 0;
    *outface = ref;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CFences::ReachabilityFence(
    /* [in] */ IInterface* ref)
{
    if (ref != NULL) {
        assert(0 && "TODO");
        // {    AutoLock syncLock(ref);}
    }
    return NOERROR;
}

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
