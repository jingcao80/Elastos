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

#ifndef __ELASTOS_DROID_OS_CUPDATELOCK_H__
#define __ELASTOS_DROID_OS_CUPDATELOCK_H__

#include "_Elastos_Droid_Os_CUpdateLock.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IIUpdateLock;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Advisory wakelock-like mechanism by which processes that should not be interrupted for
 * OTA/update purposes can so advise the OS.  This is particularly relevant for headless
 * or kiosk-like operation.
 *
 * @hide
 */
CarClass(CUpdateLock)
    , public Object
    , public IUpdateLock
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUpdateLock();

    virtual ~CUpdateLock();

    /**
     * Construct an UpdateLock instance.
     * @param tag An arbitrary string used to identify this lock instance in dump output.
     */
    CARAPI constructor(
        /* [in] */ const String& tag);

    /**
     * Change the refcount behavior of this update lock.
     */
    CARAPI SetReferenceCounted(
        /* [in] */ Boolean isRefCounted);

    /**
     * Is this lock currently held?
     */
    CARAPI IsHeld(
        /* [out] */ Boolean* result);

    /**
     * Acquire an update lock.
     */
    CARAPI AcquireLock();

    /**
     * Release this update lock.
     */
    CARAPI ReleaseLock();

private:
    static CARAPI_(void) CheckService();

    CARAPI AcquireLocked();

    CARAPI ReleaseLocked();

protected:
    AutoPtr<IBinder> mToken;
    Int32 mCount;
    Boolean mRefCounted;
    Boolean mHeld;
    String mTag;

private:
    static const Boolean DEBUG;
    static const String TAG;
    static AutoPtr<IIUpdateLock> sService;
    Object mLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CUPDATELOCK_H__
