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

#ifndef __ELASTOS_DROID_LOCATION_GEOFENCEPARAMS_H__
#define __ELASTOS_DROID_LOCATION_GEOFENCEPARAMS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Location {

class GeoFenceParams
    : public Object
    , public IGeoFenceParams
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Double lat,
        /* [in] */ Double lon,
        /* [in] */ Float r,
        /* [in] */ Int64 expire,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ const String& packageName);

    CARAPI constructor(
        /* [in] */ Int32 uid,
        /* [in] */ Double lat,
        /* [in] */ Double lon,
        /* [in] */ Float r,
        /* [in] */ Int64 expire,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ const String& packageName);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetExpiration(
        /* [out] */ Int64* expiration);

    CARAPI GetIntent(
        /* [out] */ IPendingIntent** intent);

    CARAPI GetCallerUid(
        /* [out] */ Int32* uid);

    CARAPI GetLatitude(
        /* [out] */ Double* latitude);

    CARAPI GetLongitude(
        /* [out] */ Double* longitude);

    CARAPI GetRadius(
        /* [out] */ Float* radius);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

public:
    static const Int32 ENTERING = 1;
    static const Int32 LEAVING = 2;
    /*const*/ Int32 mUid;
    /*const*/ Double mLatitude;
    /*const*/ Double mLongitude;
    /*const*/ Float mRadius;
    /*const*/ Int64 mExpiration;
    /*const*/ AutoPtr<IPendingIntent> mIntent;
    /*const*/ String mPackageName;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_GEOFENCEPARAMS_H__
