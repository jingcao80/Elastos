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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/CActivityManagerMemoryInfo.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActivityManagerMemoryInfo, Object, IActivityManagerMemoryInfo, IParcelable)

CAR_OBJECT_IMPL(CActivityManagerMemoryInfo)

CActivityManagerMemoryInfo::CActivityManagerMemoryInfo()
    : mAvailMem(0)
    , mTotalMem(0)
    , mThreshold(0)
    , mLowMemory(FALSE)
    , mHiddenAppThreshold(0)
    , mSecondaryServerThreshold(0)
    , mVisibleAppThreshold(0)
    , mForegroundAppThreshold(0)
{
}

ECode CActivityManagerMemoryInfo::constructor()
{
    return NOERROR;
}

ECode CActivityManagerMemoryInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteInt64(mAvailMem));
    FAIL_RETURN(dest->WriteInt64(mTotalMem));
    FAIL_RETURN(dest->WriteInt64(mThreshold));
    FAIL_RETURN(dest->WriteBoolean(mLowMemory));
    FAIL_RETURN(dest->WriteInt64(mHiddenAppThreshold));
    FAIL_RETURN(dest->WriteInt64(mSecondaryServerThreshold));
    FAIL_RETURN(dest->WriteInt64(mVisibleAppThreshold));
    FAIL_RETURN(dest->WriteInt64(mForegroundAppThreshold));

    return NOERROR;
}

ECode CActivityManagerMemoryInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    FAIL_RETURN(source->ReadInt64(&mAvailMem));
    FAIL_RETURN(source->ReadInt64(&mTotalMem));
    FAIL_RETURN(source->ReadInt64(&mThreshold));
    FAIL_RETURN(source->ReadBoolean(&mLowMemory));
    FAIL_RETURN(source->ReadInt64(&mHiddenAppThreshold));
    FAIL_RETURN(source->ReadInt64(&mSecondaryServerThreshold));
    FAIL_RETURN(source->ReadInt64(&mVisibleAppThreshold));
    FAIL_RETURN(source->ReadInt64(&mForegroundAppThreshold));

    return NOERROR;
}

/**
 * The available memory on the system.  This number should not
 * be considered absolute: due to the nature of the kernel, a significant
 * portion of this memory is actually in use and needed for the overall
 * system to run well.
 */
ECode CActivityManagerMemoryInfo::GetAvailMem(
    /* [out] */ Int64* availMem)
{
    VALIDATE_NOT_NULL(availMem);
    *availMem = mAvailMem;
    return NOERROR;
}

ECode CActivityManagerMemoryInfo::SetAvailMem(
    /* [in] */ Int64 availMem)
{
    mAvailMem = availMem;
    return NOERROR;
}

/**
 * The total memory accessible by the kernel.  This is basically the
 * RAM size of the device, not including below-kernel fixed allocations
 * like DMA buffers, RAM for the baseband CPU, etc.
 */
ECode CActivityManagerMemoryInfo::GetTotalMem(
    /* [out] */ Int64* totalMem)
{
    VALIDATE_NOT_NULL(totalMem);
    *totalMem = mTotalMem;
    return NOERROR;
}

ECode CActivityManagerMemoryInfo::SetTotalMem(
    /* [in] */ Int64 totalMem)
{
    mTotalMem = totalMem;
    return NOERROR;
}

/**
 * The threshold of {@link #availMem} at which we consider memory to be
 * low and start killing background services and other non-extraneous
 * processes.
 */
ECode CActivityManagerMemoryInfo::GetThreshold(
    /* [out] */ Int64* threshold)
{
    VALIDATE_NOT_NULL(threshold);
    *threshold = mThreshold;
    return NOERROR;
}

ECode CActivityManagerMemoryInfo::SetThreshold(
    /* [in] */ Int64 threshold)
{
    mThreshold = threshold;
    return NOERROR;
}

/**
 * Set to true if the system considers itself to currently be in a low
 * memory situation.
 */
ECode CActivityManagerMemoryInfo::GetLowMemory(
    /* [out] */ Boolean* lowMemory)
{
    VALIDATE_NOT_NULL(lowMemory);
    *lowMemory = mLowMemory;
    return NOERROR;
}

ECode CActivityManagerMemoryInfo::SetLowMemory(
    /* [in] */ Boolean lowMemory)
{
    mLowMemory = lowMemory;
    return NOERROR;
}

/** @hide */
ECode CActivityManagerMemoryInfo::GetHiddenAppThreshold(
    /* [out] */ Int64* threshold)
{
    VALIDATE_NOT_NULL(threshold);
    *threshold = mHiddenAppThreshold;
    return NOERROR;
}

ECode CActivityManagerMemoryInfo::SetHiddenAppThreshold(
    /* [in] */ Int64 threshold)
{
    mHiddenAppThreshold = threshold;
    return NOERROR;
}

/** @hide */
ECode CActivityManagerMemoryInfo::GetSecondaryServerThreshold(
    /* [out] */ Int64* threshold)
{
    VALIDATE_NOT_NULL(threshold);
    *threshold = mSecondaryServerThreshold;
    return NOERROR;
}

ECode CActivityManagerMemoryInfo::SetSecondaryServerThreshold(
    /* [in] */ Int64 threshold)
{
    mSecondaryServerThreshold = threshold;
    return NOERROR;
}

/** @hide */
ECode CActivityManagerMemoryInfo::GetVisibleAppThreshold(
    /* [out] */ Int64* threshold)
{
    VALIDATE_NOT_NULL(threshold);
    *threshold = mVisibleAppThreshold;
    return NOERROR;
}

ECode CActivityManagerMemoryInfo::SetVisibleAppThreshold(
    /* [in] */ Int64 threshold)
{
    mVisibleAppThreshold = threshold;
    return NOERROR;
}

/** @hide */
ECode CActivityManagerMemoryInfo::GetForegroundAppThreshold(
    /* [out] */ Int64* threshold)
{
    VALIDATE_NOT_NULL(threshold);
    *threshold = mForegroundAppThreshold;
    return NOERROR;
}

ECode CActivityManagerMemoryInfo::SetForegroundAppThreshold(
    /* [in] */ Int64 threshold)
{
    mForegroundAppThreshold = threshold;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
