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

#ifndef __ELASTOS_DROID_INTERNAL_LOCATION_CPROVIDERREQUEST_H__
#define __ELASTOS_DROID_INTERNAL_LOCATION_CPROVIDERREQUEST_H__

#include "_Elastos_Droid_Internal_Location_CProviderRequest.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Location::ILocationRequest;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Location {

/** @hide */
CarClass(CProviderRequest)
    , public Object
    , public IProviderRequest
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    //@Override
    CARAPI ToString(
        /* [out] */ String* strOut);

    CARAPI GetReportLocation(
        /* [out] */ Boolean* reportLocation);

    CARAPI SetReportLocation(
        /* [in] */ Boolean reportLocation);

    CARAPI GetInterval(
        /* [out] */ Int64* interval);

    CARAPI SetInterval(
        /* [in] */ Int64 interval);

    CARAPI GetLocationRequests(
        /* [out] */ IList** requests);

    CARAPI SetLocationRequests(
        /* [in] */ IList* requests);

public:
    /** Location reporting is requested (true) */
    Boolean mReportLocation;// = false;

    /** The smallest requested interval */
    Int64 mInterval;// = Long.MAX_VALUE;

    /**
     * A more detailed set of requests.
     * <p>Location Providers can optionally use this to
     * fine tune location updates, for example when there
     * is a high power slow interval request and a
     * low power fast interval request.
     */
    AutoPtr<IList> mLocationRequests;

};

} // namespace Location
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_LOCATION_CPROVIDERREQUEST_H__
