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

#include "elastos/droid/telephony/CDataConnectionRealTimeInfo.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL_2(CDataConnectionRealTimeInfo, Object, IDataConnectionRealTimeInfo, IParcelable)

CAR_OBJECT_IMPL(CDataConnectionRealTimeInfo)

CDataConnectionRealTimeInfo::CDataConnectionRealTimeInfo()
    : mTime(Elastos::Core::Math::INT64_MAX_VALUE)
    , mDcPowerState(DC_POWER_STATE_UNKNOWN)
{
}

ECode CDataConnectionRealTimeInfo::constructor()
{
    return NOERROR;
}

ECode CDataConnectionRealTimeInfo::constructor(
    /* [in] */ Int64 time,
    /* [in] */ Int32 dcPowerState)
{
    mTime = time;
    mDcPowerState = dcPowerState;
    return NOERROR;
}

ECode CDataConnectionRealTimeInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mTime);
    source->ReadInt32(&mDcPowerState);
    return NOERROR;
}

ECode CDataConnectionRealTimeInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mTime);
    dest->WriteInt32(mDcPowerState);
    return NOERROR;
}

ECode CDataConnectionRealTimeInfo::GetTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTime;
    return NOERROR;
}

ECode CDataConnectionRealTimeInfo::GetDcPowerState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDcPowerState;
    return NOERROR;
}

ECode CDataConnectionRealTimeInfo::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    Int64 prime = 17;
    Int64 result = 1;
    result = (prime * result) + mTime;
    result += (prime * result) + mDcPowerState;
    *hashCode = (Int32)result;

    return NOERROR;
}

ECode CDataConnectionRealTimeInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (obj == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    if(IDataConnectionRealTimeInfo::Probe(obj) == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    AutoPtr<IDataConnectionRealTimeInfo> other = IDataConnectionRealTimeInfo::Probe(obj);
    Int64 time;
    Int32 state;
    other->GetTime(&time);
    other->GetDcPowerState(&state);
    *res = (mTime == time) && (mDcPowerState == state);
    return NOERROR;
}

ECode CDataConnectionRealTimeInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append(" mTime=");
    sb.Append(mTime);
    sb.Append(" mDcPowerState=");
    sb.Append(mDcPowerState);

    *str = sb.ToString();
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
