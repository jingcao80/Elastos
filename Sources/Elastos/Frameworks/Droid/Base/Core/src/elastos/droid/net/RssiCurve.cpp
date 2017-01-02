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

#include "elastos/droid/net/RssiCurve.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(RssiCurve, Object, IParcelable, IRssiCurve)

RssiCurve::RssiCurve()
    : mStart(0)
    , mBucketWidth(0)
{}

ECode RssiCurve::constructor()
{
    return NOERROR;
}

ECode RssiCurve::constructor(
    /* [in] */ Int32 start,
    /* [in] */ Int32 bucketWidth,
    /* [in] */ ArrayOf<Byte>* rssiBuckets)
{
    mStart = start;
    mBucketWidth = bucketWidth;
    if (rssiBuckets == NULL) {
        Logger::E("RssiCurve", "rssiBuckets must be at least one element large.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (rssiBuckets->GetLength() == 0) {
        Logger::E("RssiCurve", "rssiBuckets must be at least one element large.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRssiBuckets = rssiBuckets;
    return NOERROR;
}

ECode RssiCurve::LookupScore(
    /* [in] */ Int32 rssi,
    /* [out] */ Byte* result)
{
    Int32 index = (rssi - mStart) / mBucketWidth;
    // Snap the index to the closest bucket if it falls outside the curve.
    if (index < 0) {
        index = 0;
    } else if (index > mRssiBuckets->GetLength() - 1) {
        index = mRssiBuckets->GetLength() - 1;
    }
    FUNC_RETURN((*mRssiBuckets)[index])
}

ECode RssiCurve::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) FUNC_RETURN(TRUE)
    ClassID this_cid, o_cid;
    IObject::Probe(TO_IINTERFACE(this))->GetClassID(&this_cid);
    IObject::Probe(obj)->GetClassID(&o_cid);
    if (obj == NULL || this_cid != o_cid) FUNC_RETURN(FALSE)
    AutoPtr<RssiCurve> rssiCurve = (RssiCurve*) IRssiCurve::Probe(obj);
    *result = mStart == rssiCurve->mStart &&
            mBucketWidth == rssiCurve->mBucketWidth &&
            Arrays::Equals(mRssiBuckets, rssiCurve->mRssiBuckets);
    return NOERROR;
}

ECode RssiCurve::GetHashCode(
    /* [out] */ Int32* result)
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(3);
    (*array)[0] = mStart;
    (*array)[1] = mBucketWidth;
    (*array)[2] = Arrays::GetHashCode(mRssiBuckets);
    *result = Arrays::GetHashCode(array);
    return NOERROR;
}

ECode RssiCurve::ToString(
    /* [out] */ String* result)
{
    StringBuilder sb;
    sb.Append("RssiCurve[start=");
    sb.Append(mStart);
    sb.Append(",bucketWidth=");
    sb.Append(mBucketWidth);
    sb.Append(",buckets=");
    for (Int32 i = 0; i < mRssiBuckets->GetLength(); i++) {
        sb.Append((*mRssiBuckets)[i]);
        if (i < mRssiBuckets->GetLength() - 1) {
            sb.Append(",");
        }
    }
    sb.Append("]");
    *result = sb.ToString();
    return NOERROR;
}

ECode RssiCurve::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadInt32(&mStart);
    parcel->ReadInt32(&mBucketWidth);
    parcel->ReadArrayOf((Handle32*)&mRssiBuckets);
    return NOERROR;
}

ECode RssiCurve::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mStart);
    dest->WriteInt32(mBucketWidth);
    dest->WriteArrayOf((Handle32)mRssiBuckets.Get());
    return NOERROR;
}

ECode RssiCurve::GetStart(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mStart;
    return NOERROR;
}

ECode RssiCurve::GetBucketWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mBucketWidth;
    return NOERROR;
}

ECode RssiCurve::GetRssiBuckets(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRssiBuckets;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
