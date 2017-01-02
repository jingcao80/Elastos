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

#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTTIMEVAL_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTTIMEVAL_H__

#include "core/Object.h"
#include "_Elastos_Droid_System_CStructTimeval.h"


namespace Elastos {
namespace Droid {
namespace System {

/**
 * Corresponds to C's {@code struct timeval} from
 * <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_time.h.html">&lt;sys/time.h&gt;</a>
 */
CarClass(CStructTimeval)
    , public Object
    , public IStructTimeval
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetSec(
        /* [out] */ Int64* sec);

    CARAPI GetUsec(
        /* [out] */ Int64* usec);

    CARAPI ToMillis(
        /* [out] */ Int64* millis);

    CARAPI constructor(
        /* [in] */ Int64 sec,
        /* [in] */ Int64 usec);

    static CARAPI FromMillis(
        /* [in] */ Int64 millis,
        /* [out] */ IStructTimeval** tv);

private:
    /** Seconds. */
    Int64 mTv_sec;

    /** Microseconds. */
    Int64 mTv_usec;
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEM_CSTRUCTTIMEVAL_H__
