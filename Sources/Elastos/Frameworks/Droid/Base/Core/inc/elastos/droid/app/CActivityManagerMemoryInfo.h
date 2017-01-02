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

#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGERMEMORYINFO_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGERMEMORYINFO_H__

#include "_Elastos_Droid_App_CActivityManagerMemoryInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Information you can retrieve about the available memory through
 * {@link ActivityManager#getMemoryInfo}.
 */
CarClass(CActivityManagerMemoryInfo)
    , public Object
    , public IActivityManagerMemoryInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActivityManagerMemoryInfo();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * The available memory on the system.  This number should not
     * be considered absolute: due to the nature of the kernel, a significant
     * portion of this memory is actually in use and needed for the overall
     * system to run well.
     */
    CARAPI GetAvailMem(
        /* [out] */ Int64* availMem);

    CARAPI SetAvailMem(
        /* [in] */ Int64 availMem);

    /**
     * The total memory accessible by the kernel.  This is basically the
     * RAM size of the device, not including below-kernel fixed allocations
     * like DMA buffers, RAM for the baseband CPU, etc.
     */
    CARAPI GetTotalMem(
        /* [out] */ Int64* totalMem);

    CARAPI SetTotalMem(
        /* [in] */ Int64 totalMem);

    /**
     * The threshold of {@link #availMem} at which we consider memory to be
     * low and start killing background services and other non-extraneous
     * processes.
     */
    CARAPI GetThreshold(
        /* [out] */ Int64* threshold);

    CARAPI SetThreshold(
        /* [in] */ Int64 threshold);

    /**
     * Set to true if the system considers itself to currently be in a low
     * memory situation.
     */
    CARAPI GetLowMemory(
        /* [out] */ Boolean* lowMemory);

    CARAPI SetLowMemory(
        /* [in] */ Boolean lowMemory);

    /** @hide */
    CARAPI GetHiddenAppThreshold(
        /* [out] */ Int64* threshold);

    CARAPI SetHiddenAppThreshold(
        /* [in] */ Int64 threshold);

    /** @hide */
    CARAPI GetSecondaryServerThreshold(
        /* [out] */ Int64* threshold);

    CARAPI SetSecondaryServerThreshold(
        /* [in] */ Int64 threshold);

    /** @hide */
    CARAPI GetVisibleAppThreshold(
        /* [out] */ Int64* threshold);

    CARAPI SetVisibleAppThreshold(
        /* [in] */ Int64 threshold);

    /** @hide */
    CARAPI GetForegroundAppThreshold(
        /* [out] */ Int64* threshold);

    CARAPI SetForegroundAppThreshold(
        /* [in] */ Int64 threshold);

private:
    Int64 mAvailMem;

    Int64 mTotalMem;

    Int64 mThreshold;

    Boolean mLowMemory;

    /** @hide */
    Int64 mHiddenAppThreshold;
    /** @hide */
    Int64 mSecondaryServerThreshold;
    /** @hide */
    Int64 mVisibleAppThreshold;
    /** @hide */
    Int64 mForegroundAppThreshold;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYMANAGERMEMORYINFO_H__
