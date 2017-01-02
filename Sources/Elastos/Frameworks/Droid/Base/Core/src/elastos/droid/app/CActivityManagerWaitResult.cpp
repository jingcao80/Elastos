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
#include "elastos/droid/app/CActivityManagerWaitResult.h"
#include "elastos/droid/content/CComponentName.h"

using Elastos::Droid::Content::CComponentName;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActivityManagerWaitResult, Object, IActivityManagerWaitResult, IParcelable)

CAR_OBJECT_IMPL(CActivityManagerWaitResult)

CActivityManagerWaitResult::CActivityManagerWaitResult()
    : mResult(0)
    , mTimeout(FALSE)
    , mThisTime(0)
    , mTotalTime(0)
{
}

ECode CActivityManagerWaitResult::constructor()
{
    return NOERROR;
}

ECode CActivityManagerWaitResult::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    dest->WriteInt32(mResult);
    dest->WriteInt32(mTimeout ? 1 : 0);

    IParcelable* parcelable = IParcelable::Probe(mWho);
    dest->WriteInt32(parcelable ? 1 : 0);
    if (parcelable) {
        parcelable->WriteToParcel(dest);
    }

    dest->WriteInt64(mThisTime);
    dest->WriteInt64(mTotalTime);

    return NOERROR;
}

ECode CActivityManagerWaitResult::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    source->ReadInt32(&mResult);
    Int32 value;
    source->ReadInt32(&value);
    mTimeout = value != 0;

    mWho = NULL;
    source->ReadInt32(&value);
    if (value == 1) {
        CComponentName::New((IComponentName**)&mWho);
        IParcelable* parcelable = IParcelable::Probe(mWho);
        parcelable->ReadFromParcel(source);
    }

    source->ReadInt64(&mThisTime);
    source->ReadInt64(&mTotalTime);
    return NOERROR;
}

ECode CActivityManagerWaitResult::GetResult(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResult;
    return NOERROR;
}

ECode CActivityManagerWaitResult::SetResult(
    /* [in] */ Int32 result)
{
    mResult= result;
    return NOERROR;
}

ECode CActivityManagerWaitResult::GetTimeout(
    /* [out] */ Boolean* timeout)
{
    VALIDATE_NOT_NULL(timeout);
    *timeout = mTimeout;
    return NOERROR;
}

ECode CActivityManagerWaitResult::SetTimeout(
    /* [in] */ Boolean timeout)
{
    mTimeout= timeout;
    return NOERROR;
}

ECode CActivityManagerWaitResult::GetWho(
    /* [out] */ IComponentName** who)
{
    VALIDATE_NOT_NULL(who);
    *who = mWho;
    REFCOUNT_ADD(*who);
    return NOERROR;
}

ECode CActivityManagerWaitResult::SetWho(
    /* [in] */ IComponentName* who)
{
    mWho = who;
    return NOERROR;
}

ECode CActivityManagerWaitResult::GetThisTime(
    /* [out] */ Int64* thisTime)
{
    VALIDATE_NOT_NULL(thisTime);
    *thisTime = mThisTime;
    return NOERROR;
}

ECode CActivityManagerWaitResult::SetThisTime(
    /* [in] */ Int64 thisTime)
{
    mThisTime = thisTime;
    return NOERROR;
}

ECode CActivityManagerWaitResult::GetTotalTime(
    /* [out] */ Int64* totalTime)
{
    VALIDATE_NOT_NULL(totalTime);
    *totalTime = mTotalTime;
    return NOERROR;
}

ECode CActivityManagerWaitResult::SetTotalTime(
    /* [in] */ Int64 totalTime)
{
    mTotalTime = totalTime;
    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos
