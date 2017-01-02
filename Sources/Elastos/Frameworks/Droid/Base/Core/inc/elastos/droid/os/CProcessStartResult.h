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

#ifndef __ELASTOS_DROID_OS_CPROCESSSTARTRESULT_H__
#define __ELASTOS_DROID_OS_CPROCESSSTARTRESULT_H__

#include "_Elastos_Droid_Os_CProcessStartResult.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CProcessStartResult)
    , public Object
    , public IProcessStartResult
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CProcessStartResult();

    CARAPI GetPid(
        /* [out] */ Int32* pid);

    CARAPI SetPid(
        /* [in] */ Int32 pid);

    CARAPI GetUsingWrapper(
        /* [out] */ Boolean* usingWrapper);

    CARAPI SetUsingWrapper(
        /* [in] */ Boolean usingWrapper);

private:
    /**
     * The PID of the newly started process.
     * Always >= 0.  (If the start failed, an exception will have been thrown instead.)
     */
    Int32 mPid;

    /**
     * True if the process was started with a wrapper attached.
     */
    Boolean mUsingWrapper;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CPROCESSSTARTRESULT_H__
