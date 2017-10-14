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

#include "elastos/droid/telephony/CellInfo.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::Math;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telephony {

const Int32 CellInfo::TYPE_GSM = 1;
const Int32 CellInfo::TYPE_CDMA = 2;
const Int32 CellInfo::TYPE_LTE = 3;
const Int32 CellInfo::TYPE_WCDMA = 4;

CAR_INTERFACE_IMPL_2(CellInfo, Object, ICellInfo, IParcelable)

CellInfo::CellInfo()
    : mRegistered(FALSE)
    , mTimeStamp(Elastos::Core::Math::INT64_MAX_VALUE)
    , mTimeStampType(ICellInfo::TIMESTAMP_TYPE_UNKNOWN)
{}

ECode CellInfo::constructor()
{
    return NOERROR;
}

ECode CellInfo::constructor(
    /* [in] */ ICellInfo* ci)
{
    Boolean isRegistered;
    Int64 timeStamp;
    Int32 timeStampType;
    ci->IsRegistered(&isRegistered);
    ci->GetTimeStamp(&timeStamp);
    ci->GetTimeStampType(&timeStampType);
    mRegistered = isRegistered;
    mTimeStampType = timeStampType;
    mTimeStamp = timeStamp;
    return NOERROR;
}

ECode CellInfo::IsRegistered(
    /* [out] */ Boolean* res)
{
    *res = mRegistered;
    return NOERROR;
}

ECode CellInfo::SetRegistered(
    /* [in] */ Boolean registered)
{
    mRegistered = registered;
    return NOERROR;
}

ECode CellInfo::GetTimeStamp(
    /* [out] */ Int64* timeStamp)
{
    *timeStamp = mTimeStamp;
    return NOERROR;
}

ECode CellInfo::SetTimeStamp(
    /* [in] */ Int64 timeStamp)
{
    mTimeStamp = timeStamp;
    return NOERROR;
}

ECode CellInfo::GetTimeStampType(
    /* [out] */ Int32* timeStampType)
{
    *timeStampType = mTimeStampType;
    return NOERROR;
}

ECode CellInfo::SetTimeStampType(
    /* [in] */ Int32 timeStampType)
{
    if (timeStampType < ICellInfo::TIMESTAMP_TYPE_UNKNOWN || timeStampType > ICellInfo::TIMESTAMP_TYPE_JAVA_RIL) {
        mTimeStampType = ICellInfo::TIMESTAMP_TYPE_UNKNOWN;
    }
    else {
        mTimeStampType = timeStampType;
    }
    return NOERROR;
}

ECode CellInfo::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    Int32 primeNum = 31;
    *hashCode = ((mRegistered ? 0 : 1) * primeNum) + ((Int32)(mTimeStamp / 1000) * primeNum)
            + (mTimeStampType * primeNum);
    return NOERROR;
}

ECode CellInfo::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    if (other == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    if ((ICellInfo*)this == ICellInfo::Probe(other)) {
        *result = TRUE;
        return NOERROR;
    }
    AutoPtr<ICellInfo> o = ICellInfo::Probe(other);
    if (o != NULL) {
        Boolean isRegistered;
        Int64 timeStamp;
        Int32 timeStampType;
        o->IsRegistered(&isRegistered);
        o->GetTimeStamp(&timeStamp);
        o->GetTimeStampType(&timeStampType);
        *result = (mRegistered == isRegistered && mTimeStamp == timeStamp && mTimeStampType == timeStampType);
    }
    else *result = FALSE;
    return NOERROR;
}

String CellInfo::TimeStampTypeToString(
    /* [in] */ Int32 type)
{
    switch (type) {
        case 1:
            return String("antenna");
        case 2:
            return String("modem");
        case 3:
            return String("oem_ril");
        case 4:
            return String("java_ril");
        default:
            return String("unknown");
    }
}

ECode CellInfo::ToString(
    /* [out] */ String* str)
{
    StringBuilder sb;
    String timeStampType;

    sb.Append("mRegistered=");
    sb.Append(mRegistered ? "YES" : "NO");
    timeStampType = TimeStampTypeToString(mTimeStampType);
    sb.Append(" mTimeStampType=");
    sb.Append(timeStampType);
    sb.Append(" mTimeStamp=");
    sb.Append(mTimeStamp);
    sb.Append("ns");
    *str = sb.ToString();
    return NOERROR;
}

ECode CellInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadBoolean(&mRegistered);
    source->ReadInt32(&mTimeStampType);
    source->ReadInt64(&mTimeStamp);
    return NOERROR;
}

ECode CellInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteBoolean(mRegistered);
    dest->WriteInt32(mTimeStampType);
    dest->WriteInt64(mTimeStamp);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
