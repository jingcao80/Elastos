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

#include "elastos/droid/app/usage/CUsageStats.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Usage {

CAR_INTERFACE_IMPL_2(CUsageStats, Object, IUsageStats, IParcelable)

CAR_OBJECT_IMPL(CUsageStats)

CUsageStats::CUsageStats()
    : mBeginTimeStamp(0)
    , mEndTimeStamp(0)
    , mLastTimeUsed(0)
    , mTotalTimeInForeground(0)
    , mLaunchCount(0)
    , mLastEvent(0)
{}

ECode CUsageStats::constructor()
{
    return NOERROR;
}

ECode CUsageStats::constructor(
    /* [in] */ IUsageStats* stats)
{
    CUsageStats* other = (CUsageStats*)stats;
    mPackageName = other->mPackageName;
    mBeginTimeStamp = other->mBeginTimeStamp;
    mEndTimeStamp = other->mEndTimeStamp;
    mLastTimeUsed = other->mLastTimeUsed;
    mTotalTimeInForeground = other->mTotalTimeInForeground;
    mLaunchCount = other->mLaunchCount;
    mLastEvent = other->mLastEvent;
    return NOERROR;
}

ECode CUsageStats::GetPackageName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPackageName;
    return NOERROR;
}

ECode CUsageStats::GetFirstTimeStamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBeginTimeStamp;
    return NOERROR;
}

ECode CUsageStats::GetLastTimeStamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEndTimeStamp;
    return NOERROR;
}

ECode CUsageStats::GetLastTimeUsed(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLastTimeUsed;
    return NOERROR;
}

ECode CUsageStats::GetTotalTimeInForeground(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTotalTimeInForeground;
    return NOERROR;
}

ECode CUsageStats::Add(
    /* [in] */ IUsageStats* other)
{
    CUsageStats* right = (CUsageStats*)other;
    if (!mPackageName.Equals(right->mPackageName)) {
        Logger::E("", "Can't merge UsageStats for package '%s' with UsageStats for package '%s'.",
            mPackageName.string(), right->mPackageName.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (right->mEndTimeStamp > mEndTimeStamp) {
        mLastEvent = right->mLastEvent;
        mEndTimeStamp = right->mEndTimeStamp;
        mLastTimeUsed = right->mLastTimeUsed;
    }
    mBeginTimeStamp = Elastos::Core::Math::Min(mBeginTimeStamp, right->mBeginTimeStamp);
    mTotalTimeInForeground += right->mTotalTimeInForeground;
    mLaunchCount += right->mLaunchCount;
    return NOERROR;
}

ECode CUsageStats::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mPackageName);
    dest->WriteInt64(mBeginTimeStamp);
    dest->WriteInt64(mEndTimeStamp);
    dest->WriteInt64(mLastTimeUsed);
    dest->WriteInt64(mTotalTimeInForeground);
    dest->WriteInt32(mLaunchCount);
    dest->WriteInt32(mLastEvent);
    return NOERROR;
}

ECode CUsageStats::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadString(&mPackageName);
    in->ReadInt64(&mBeginTimeStamp);
    in->ReadInt64(&mEndTimeStamp);
    in->ReadInt64(&mLastTimeUsed);
    in->ReadInt64(&mTotalTimeInForeground);
    in->ReadInt32(&mLaunchCount);
    in->ReadInt32(&mLastEvent);
    return NOERROR;
}

} // namespace Usage
} // namespace App
} // namespace Droid
} // namespace Elastos
