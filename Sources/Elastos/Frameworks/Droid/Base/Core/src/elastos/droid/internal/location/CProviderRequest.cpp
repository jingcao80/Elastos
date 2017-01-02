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

#include "elastos/droid/internal/location/CProviderRequest.h"
#include "elastos/droid/location/CLocationRequest.h"
#include "elastos/droid/utility/TimeUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Location::CLocationRequest;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Location {

CAR_OBJECT_IMPL(CProviderRequest);

CAR_INTERFACE_IMPL_2(CProviderRequest, Object, IProviderRequest, IParcelable);

ECode CProviderRequest::constructor()
{
    mReportLocation = FALSE;
    mInterval = Elastos::Core::Math::INT64_MAX_VALUE;
    CArrayList::New((IList**)&mLocationRequests);
    return NOERROR;
}

ECode CProviderRequest::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);

    parcel->ReadBoolean(&mReportLocation);
    parcel->ReadInt64(&mInterval);

    Int32 count = 0;
    parcel->ReadInt32(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<ILocationRequest> request;
        CLocationRequest::New((ILocationRequest**)&request);
        IParcelable::Probe(request)->ReadFromParcel(parcel);
        mLocationRequests->Add(request);
    }

    return NOERROR;
}

//@Override
ECode CProviderRequest::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);

    parcel->WriteBoolean(mReportLocation);
    parcel->WriteInt64(mInterval);
    Int32 size;
    mLocationRequests->GetSize(&size);
    parcel->WriteInt32(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        mLocationRequests->Get(i, (IInterface**)&item);
        IParcelable::Probe(item)->WriteToParcel(parcel);
    }

    return NOERROR;
}

ECode CProviderRequest::GetReportLocation(
    /* [out] */ Boolean* reportLocation)
{
    VALIDATE_NOT_NULL(reportLocation);
    *reportLocation = mReportLocation;
    return NOERROR;
}

ECode CProviderRequest::SetReportLocation(
    /* [in] */ Boolean reportLocation)
{
    mReportLocation = reportLocation;
    return NOERROR;
}

ECode CProviderRequest::GetInterval(
    /* [out] */ Int64* interval)
{
    VALIDATE_NOT_NULL(interval);
    *interval = mInterval;
    return NOERROR;
}

ECode CProviderRequest::SetInterval(
    /* [in] */ Int64 interval)
{
    mInterval = interval;
    return NOERROR;
}

ECode CProviderRequest::GetLocationRequests(
    /* [out] */ IList** requests)
{
    VALIDATE_NOT_NULL(requests);
    *requests = mLocationRequests;
    REFCOUNT_ADD(*requests);
    return NOERROR;
}

ECode CProviderRequest::SetLocationRequests(
    /* [in] */ IList* requests)
{
    mLocationRequests = requests;
    return NOERROR;
}

//@Override
ECode CProviderRequest::ToString(
    /* [out] */ String* strOut)
{
    VALIDATE_NOT_NULL(strOut);

    StringBuilder sb("ProviderRequest[");
    if (mReportLocation) {
        sb += "ON";
        sb += " mInterval=";
        TimeUtils::FormatDuration(mInterval, sb);
    }
    else {
        sb += "OFF";
    }
    sb += "]";

    *strOut = sb.ToString();

    return NOERROR;
}

} // namespace Location
} // namespace Internal
} // namespace Droid
} // namespace Elastos
