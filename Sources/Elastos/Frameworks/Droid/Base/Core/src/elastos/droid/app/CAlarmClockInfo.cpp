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

#include "elastos/droid/app/CAlarmClockInfo.h"
#include "elastos/droid/app/CPendingIntent.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CAlarmClockInfo, Object, IAlarmClockInfo, IParcelable)

CAR_OBJECT_IMPL(CAlarmClockInfo)

CAlarmClockInfo::CAlarmClockInfo()
    : mTriggerTime(0)
{}

ECode CAlarmClockInfo::constructor()
{
    return NOERROR;
}

ECode CAlarmClockInfo::constructor(
    /* [in] */ Int64 triggerTime,
    /* [in] */ IPendingIntent* showIntent)
{
    mTriggerTime = triggerTime;
    mShowIntent = showIntent;
    return NOERROR;
}

ECode CAlarmClockInfo::GetTriggerTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTriggerTime;
    return NOERROR;
}

ECode CAlarmClockInfo::SetTriggerTime(
    /* [in] */ Int64 value)
{
    mTriggerTime = value;
    return NOERROR;
}

ECode CAlarmClockInfo::GetShowIntent(
    /* [out] */ IPendingIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mShowIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CAlarmClockInfo::SetShowIntent(
    /* [in] */ IPendingIntent* intent)
{
    mShowIntent = intent;
    return NOERROR;
}

ECode CAlarmClockInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mTriggerTime);
    IParcelable::Probe(mShowIntent)->WriteToParcel(dest);
    return NOERROR;
}

ECode CAlarmClockInfo::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt64(&mTriggerTime);
    CPendingIntent::New((IPendingIntent**)&mShowIntent);
    IParcelable::Probe(mShowIntent)->ReadFromParcel(in);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
