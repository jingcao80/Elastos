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

#ifndef __ELASTOS_DROID_OS_CDEBUGMEMORYINFO_H__
#define __ELASTOS_DROID_OS_CDEBUGMEMORYINFO_H__

#include "_Elastos_Droid_Os_CDebugMemoryInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * This class is used to retrieved various statistics about the memory mappings for this
 * process. The returns info broken down by dalvik, native, and other. All results are in kB.
 */
CarClass(CDebugMemoryInfo)
    , public Object
    , public IDebugMemoryInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDebugMemoryInfo();

    ~CDebugMemoryInfo();

    CARAPI constructor();

    CARAPI GetTotalUss(
        /* [out] */ Int32* totalUss);

    CARAPI GetTotalSwappablePss(
        /* [out] */ Int32* totalUss);

    CARAPI GetTotalPss(
        /* [out] */ Int32* totalPss);

    CARAPI GetTotalPrivateDirty(
        /* [out] */ Int32* totalPrivateDirty);

    CARAPI GetTotalSharedDirty(
        /* [out] */ Int32* totalSharedDirty);

    CARAPI GetOtherPss(
        /* [in] */ Int32 which,
        /* [out] */ Int32* otherPss);

    CARAPI GetOtherPrivateDirty(
        /* [in] */ Int32 which,
        /* [out] */ Int32* otherPrivateDirty);

    CARAPI GetOtherSharedDirty(
        /* [in] */ Int32 which,
        /* [out] */ Int32* otherSharedDirty);

    CARAPI GetDalvikPss(
        /* [out] */ Int32* dalvikPss);

    CARAPI SetDalvikPss(
        /* [in] */ Int32 mDalvikPss);

    CARAPI GetDalvikPrivateDirty(
        /* [out] */ Int32* dalvikPrivateDirty);

    CARAPI SetDalvikPrivateDirty(
        /* [in] */ Int32 dalvikPrivateDirty);

    CARAPI GetDalvikSharedDirty(
        /* [out] */ Int32* dalvikSharedDirty);

    CARAPI SetDalvikSharedDirty(
        /* [in] */ Int32 dalvikSharedDirty);

    CARAPI GetNativePss(
        /* [out] */ Int32* nativePss);

    CARAPI SetNativePss(
        /* [in] */ Int32 nativePss);

    CARAPI GetNativePrivateDirty(
        /* [out] */ Int32* nativePrivateDirty);

    CARAPI SetNativePrivateDirty(
        /* [in] */ Int32 nativePrivateDirty);

    CARAPI GetNativeSharedDirty(
        /* [out] */ Int32* nativeSharedDirty);

    CARAPI SetNativeSharedDirty(
        /* [in] */ Int32 nativeSharedDirty);

    CARAPI GetOtherPss(
        /* [out] */ Int32* otherPss);

    CARAPI SetOtherPss(
        /* [in] */ Int32 otherPss);

    CARAPI GetOtherPrivateDirty(
        /* [out] */ Int32* otherPrivateDirty);

    CARAPI SetOtherPrivateDirty(
        /* [in] */ Int32 otherPrivateDirty);

    CARAPI GetOtherSharedDirty(
        /* [out] */ Int32* otherSharedDirty);

    CARAPI SetOtherSharedDirty(
        /* [in] */ Int32 otherSharedDirty);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    friend class Debug;

    CARAPI SetOtherStats(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    CARAPI GetOtherStats(
        /* [out] */ ArrayOf<Int32>** stats);

public:
    static CARAPI GetOtherLabel(
        /* [in] */ Int32 which,
        /* [out] */ String* label);

public:
    /** The proportional set size for dalvik heap.  (Doesn't include other Dalvik overhead.) */
    Int32 mDalvikPss;

    /** The proportional set size that is swappable for dalvik heap. */
    /** @hide We may want to expose this, eventually. */
    Int32 mDalvikSwappablePss;

    /** The private dirty pages used by dalvik heap. */
    Int32 mDalvikPrivateDirty;

    /** The shared dirty pages used by dalvik heap. */
    Int32 mDalvikSharedDirty;

    /** The private clean pages used by dalvik heap. */
    /** @hide We may want to expose this, eventually. */
    Int32 mDalvikPrivateClean;

    /** The shared clean pages used by dalvik heap. */
    /** @hide We may want to expose this, eventually. */
    Int32 mDalvikSharedClean;

    /** The dirty dalvik pages that have been swapped out. */
    /** @hide We may want to expose this, eventually. */
    Int32 mDalvikSwappedOut;

    /** The proportional set size for the native heap. */
    Int32 mNativePss;

    /** The proportional set size that is swappable for the native heap. */
    /** @hide We may want to expose this, eventually. */
    Int32 mNativeSwappablePss;

    /** The private dirty pages used by the native heap. */
    Int32 mNativePrivateDirty;

    /** The shared dirty pages used by the native heap. */
    Int32 mNativeSharedDirty;

    /** The private clean pages used by the native heap. */
    /** @hide We may want to expose this, eventually. */
    Int32 mNativePrivateClean;

    /** The shared clean pages used by the native heap. */
    /** @hide We may want to expose this, eventually. */
    Int32 mNativeSharedClean;

    /** The dirty native pages that have been swapped out. */
    /** @hide We may want to expose this, eventually. */
    Int32 mNativeSwappedOut;

    /** The proportional set size for everything else. */
    Int32 mOtherPss;

    /** The proportional set size that is swappable for everything else. */
    /** @hide We may want to expose this, eventually. */
    Int32 mOtherSwappablePss;

    /** The private dirty pages used by everything else. */
    Int32 mOtherPrivateDirty;

    /** The shared dirty pages used by everything else. */
    Int32 mOtherSharedDirty;

    /** The private clean pages used by everything else. */
    /** @hide We may want to expose this, eventually. */
    Int32 mOtherPrivateClean;

    /** The shared clean pages used by everything else. */
    /** @hide We may want to expose this, eventually. */
    Int32 mOtherSharedClean;

    /** The dirty pages used by anyting else that have been swapped out. */
    /** @hide We may want to expose this, eventually. */
    Int32 mOtherSwappedOut;

private:
    AutoPtr<ArrayOf<Int32> > mOtherStats;/* = new int[NUM_OTHER_STATS*3]*/
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CDEBUGMEMORYINFO_H__
