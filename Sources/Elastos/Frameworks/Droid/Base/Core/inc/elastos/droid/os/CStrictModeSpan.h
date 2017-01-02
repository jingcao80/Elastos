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

#ifndef __ELASTOS_DROID_OS_CSTRICTMODESPAN_H__
#define __ELASTOS_DROID_OS_CSTRICTMODESPAN_H__

#include "_Elastos_Droid_Os_CStrictModeSpan.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>
#include "elastos/droid/os/CStrictMode.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CStrictModeSpan)
    , public Object
    , public IStrictModeSpan
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Handle32 threadState);

    CARAPI Finish();

private:
    friend class CStrictMode;
    friend class CStrictModeViolationInfo;

    String mName;
    Int64 mCreateMillis;
    AutoPtr<CStrictModeSpan> mNext;
    AutoPtr<CStrictModeSpan> mPrev;  // not used when in freeList, only active
    AutoPtr<CStrictMode::ThreadSpanState> mContainerState;

    Object mContainerStateLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CSTRICTMODESPAN_H__
