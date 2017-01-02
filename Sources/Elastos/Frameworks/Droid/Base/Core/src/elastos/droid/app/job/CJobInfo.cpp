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

#include "elastos/droid/app/job/CJobInfo.h"
#include "elastos/droid/os/CPersistableBundle.h"
#include "elastos/droid/content/CComponentName.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CPersistableBundle;
using Elastos::Droid::Content::CComponentName;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Job {

//========================================================================
// CJobInfo
//========================================================================
CAR_INTERFACE_IMPL_2(CJobInfo, Object, IJobInfo, IParcelable)

CAR_OBJECT_IMPL(CJobInfo)

CJobInfo::CJobInfo()
    : mJobId(0)
    , mRequireCharging(FALSE)
    , mRequireDeviceIdle(FALSE)
    , mHasEarlyConstraint(FALSE)
    , mHasLateConstraint(FALSE)
    , mNetworkType(0)
    , mMinLatencyMillis(0)
    , mMaxExecutionDelayMillis(0)
    , mIsPeriodic(FALSE)
    , mIsPersisted(FALSE)
    , mIntervalMillis(0)
    , mInitialBackoffMillis(0)
    , mBackoffPolicy(0)
{
}

ECode CJobInfo::constructor()
{
    return NOERROR;
}

ECode CJobInfo::constructor(
    /* [in] */ IJobInfoBuilder* jib)
{
    VALIDATE_NOT_NULL(jib)
    JobInfoBuilder* b = (JobInfoBuilder*)jib;
    mJobId = b->mJobId;
    mExtras = b->mExtras;
    mService = b->mJobService;
    mRequireCharging = b->mRequiresCharging;
    mRequireDeviceIdle = b->mRequiresDeviceIdle;
    mNetworkType = b->mNetworkType;
    mMinLatencyMillis = b->mMinLatencyMillis;
    mMaxExecutionDelayMillis = b->mMaxExecutionDelayMillis;
    mIsPeriodic = b->mIsPeriodic;
    mIsPersisted = b->mIsPersisted;
    mIntervalMillis = b->mIntervalMillis;
    mInitialBackoffMillis = b->mInitialBackoffMillis;
    mBackoffPolicy = b->mBackoffPolicy;
    mHasEarlyConstraint = b->mHasEarlyConstraint;
    mHasLateConstraint = b->mHasLateConstraint;
    return NOERROR;
}

ECode CJobInfo::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mJobId;
    return NOERROR;
}

ECode CJobInfo::GetExtras(
    /* [out] */ IPersistableBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    *extras = mExtras;
    REFCOUNT_ADD(*extras)
    return NOERROR;
}

ECode CJobInfo::GetService(
    /* [out] */ IComponentName** service)
{
    VALIDATE_NOT_NULL(service)
    *service = mService;
    REFCOUNT_ADD(*service)
    return NOERROR;
}

ECode CJobInfo::IsRequireCharging(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRequireCharging;
    return NOERROR;
}

ECode CJobInfo::IsRequireDeviceIdle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRequireDeviceIdle;
    return NOERROR;
}

ECode CJobInfo::GetNetworkType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNetworkType;
    return NOERROR;
}

ECode CJobInfo::GetMinLatencyMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMinLatencyMillis;
    return NOERROR;
}

ECode CJobInfo::GetMaxExecutionDelayMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaxExecutionDelayMillis;
    return NOERROR;
}

ECode CJobInfo::IsPeriodic(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsPeriodic;
    return NOERROR;
}

ECode CJobInfo::IsPersisted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsPersisted;
    return NOERROR;
}

ECode CJobInfo::GetIntervalMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIntervalMillis;
    return NOERROR;
}

ECode CJobInfo::GetInitialBackoffMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mInitialBackoffMillis;
    return NOERROR;
}

ECode CJobInfo::GetBackoffPolicy(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBackoffPolicy;
    return NOERROR;
}

ECode CJobInfo::HasEarlyConstraint(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHasEarlyConstraint;
    return NOERROR;
}

ECode CJobInfo::HasLateConstraint(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHasLateConstraint;
    return NOERROR;
}

ECode CJobInfo::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mJobId);
    CPersistableBundle::New((IPersistableBundle**)&mExtras);
    IParcelable::Probe(mExtras)->ReadFromParcel(in);
    CComponentName::New((IComponentName**)&mService);
    IParcelable::Probe(mService)->ReadFromParcel(in);

    Int32 ival;
    in->ReadInt32(&ival);
    mRequireCharging = ival == 1;
    in->ReadInt32(&ival);
    mRequireDeviceIdle = ival == 1;
    in->ReadInt32(&mNetworkType);
    in->ReadInt64(&mMinLatencyMillis);
    in->ReadInt64(&mMaxExecutionDelayMillis);
    in->ReadInt32(&ival);
    mIsPeriodic = ival == 1;
    in->ReadInt32(&ival);
    mIsPersisted = ival == 1;
    in->ReadInt64(&mIntervalMillis);
    in->ReadInt64(&mInitialBackoffMillis);
    in->ReadInt32(&mBackoffPolicy);
    in->ReadInt32(&ival);
    mHasEarlyConstraint = ival == 1;
    in->ReadInt32(&ival);
    mHasLateConstraint = ival == 1;
    return NOERROR;
}

ECode CJobInfo::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(mJobId);
    IParcelable::Probe(mExtras)->WriteToParcel(out);
    IParcelable::Probe(mService)->WriteToParcel(out);
    out->WriteInt32(mRequireCharging ? 1 : 0);
    out->WriteInt32(mRequireDeviceIdle ? 1 : 0);
    out->WriteInt32(mNetworkType);
    out->WriteInt64(mMinLatencyMillis);
    out->WriteInt64(mMaxExecutionDelayMillis);
    out->WriteInt32(mIsPeriodic ? 1 : 0);
    out->WriteInt32(mIsPersisted ? 1 : 0);
    out->WriteInt64(mIntervalMillis);
    out->WriteInt64(mInitialBackoffMillis);
    out->WriteInt32(mBackoffPolicy);
    out->WriteInt32(mHasEarlyConstraint ? 1 : 0);
    out->WriteInt32(mHasLateConstraint ? 1 : 0);
    return NOERROR;
}

ECode CJobInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("(job:");
    sb += mJobId;
    sb += "/";
    String info;
    mService->FlattenToShortString(&info);
    sb += info;
    sb += ")";
    *str = sb.ToString();
    return NOERROR;
}

//========================================================================
// CJobInfo::JobInfoBuilder
//========================================================================

CAR_INTERFACE_IMPL(JobInfoBuilder, Object, IJobInfoBuilder)

JobInfoBuilder::JobInfoBuilder()
    : mJobId(0)
    , mRequiresCharging(FALSE)
    , mRequiresDeviceIdle(FALSE)
    , mNetworkType(0)
    , mIsPersisted(FALSE)
    , mMinLatencyMillis(0)
    , mMaxExecutionDelayMillis(0)
    , mIsPeriodic(FALSE)
    , mHasEarlyConstraint(FALSE)
    , mHasLateConstraint(FALSE)
    , mIntervalMillis(0)
    , mInitialBackoffMillis(IJobInfo::DEFAULT_INITIAL_BACKOFF_MILLIS)
    , mBackoffPolicy(IJobInfo::DEFAULT_BACKOFF_POLICY)
    , mBackoffPolicySet(FALSE)
{
    mExtras = CPersistableBundle::EMPTY;
}

JobInfoBuilder::~JobInfoBuilder()
{}

ECode JobInfoBuilder::constructor(
    /* [in] */ Int32 jobId,
    /* [in] */ IComponentName* jobService)
{
    mJobService = jobService;
    mJobId = jobId;
    return NOERROR;
}

ECode JobInfoBuilder::SetExtras(
    /* [in] */ IPersistableBundle* extras)
{
    mExtras = extras;
    return NOERROR;
}

ECode JobInfoBuilder::SetRequiredNetworkType(
    /* [in] */ Int32 mNetworkType)
{
    mNetworkType = mNetworkType;
    return NOERROR;
}

ECode JobInfoBuilder::SetRequiresCharging(
    /* [in] */ Boolean requiresCharging)
{
    mRequiresCharging = requiresCharging;
    return NOERROR;
}

ECode JobInfoBuilder::SetRequiresDeviceIdle(
    /* [in] */ Boolean requiresDeviceIdle)
{
    mRequiresDeviceIdle = requiresDeviceIdle;
    return NOERROR;
}

ECode JobInfoBuilder::SetPeriodic(
    /* [in] */ Int64 mIntervalMillis)
{
    mIsPeriodic = TRUE;
    mIntervalMillis = mIntervalMillis;
    mHasEarlyConstraint = mHasLateConstraint = TRUE;
    return NOERROR;
}

ECode JobInfoBuilder::SetMinimumLatency(
    /* [in] */ Int64 mMinLatencyMillis)
{
    mMinLatencyMillis = mMinLatencyMillis;
    mHasEarlyConstraint = TRUE;
    return NOERROR;
}

ECode JobInfoBuilder::SetOverrideDeadline(
    /* [in] */ Int64 mMaxExecutionDelayMillis)
{
    mMaxExecutionDelayMillis = mMaxExecutionDelayMillis;
    mHasLateConstraint = TRUE;
    return NOERROR;
}

ECode JobInfoBuilder::SetBackoffCriteria(
    /* [in] */ Int64 mInitialBackoffMillis,
    /* [in] */ Int32 mBackoffPolicy)
{
    mBackoffPolicySet = TRUE;
    mInitialBackoffMillis = mInitialBackoffMillis;
    mBackoffPolicy = mBackoffPolicy;
    return NOERROR;
}

ECode JobInfoBuilder::SetPersisted(
    /* [in] */ Boolean mIsPersisted)
{
    mIsPersisted = mIsPersisted;
    return NOERROR;
}

ECode JobInfoBuilder::Build(
    /* [out] */ IJobInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;

    // Allow jobs with no constraInt32s - What am I, a database?
    if (!mHasEarlyConstraint && !mHasLateConstraint && !mRequiresCharging
        && !mRequiresDeviceIdle && mNetworkType == IJobInfo::NETWORK_TYPE_NONE) {

        Logger::E("JobInfoBuilder", "You're trying to build a job with no "
            "constraInt32s, this is not allowed.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IPersistableBundle> tmp = mExtras;
    mExtras = NULL;
    CPersistableBundle::New(tmp, (IPersistableBundle**)&mExtras);  // Make our own copy.
    // Check that a deadline was not set on a periodic job.
    if (mIsPeriodic && (mMaxExecutionDelayMillis != 0)) {
        Logger::E("JobInfoBuilder", "Can't call setOverrideDeadline() on a periodic job.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mIsPeriodic && (mMinLatencyMillis != 0)) {
        Logger::E("JobInfoBuilder", "Can't call setMinimumLatency() on a periodic job.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mBackoffPolicySet && mRequiresDeviceIdle) {
        Logger::E("JobInfoBuilder", "An idle mode job will not respect any"
            " back-off policy, so calling setBackoffCriteria with"
            " setRequiresDeviceIdle is an error.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return CJobInfo::New(this, info);
}

} // namespace Job
} // namespace App
} // namespace Droid
} // namespace Elastos
