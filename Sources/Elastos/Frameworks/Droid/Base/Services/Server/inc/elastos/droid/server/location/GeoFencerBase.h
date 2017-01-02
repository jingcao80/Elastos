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

#ifndef __ELASTOS_DROID_SERVER_LOCATION_GEOFENCERBASE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GEOFENCERBASE_H__

#include "Elastos.Droid.Location.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Location::IGeoFenceParams;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * This class defines a base class for GeoFencers
 *
 * @hide
 */
class GeoFencerBase : public Object
{
public:
    GeoFencerBase();

    CARAPI_(void) Add(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Float radius,
        /* [in] */ Int64 expiration,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ const String& packageName);

    CARAPI_(void) Add(
        /* [in] */ IGeoFenceParams* geoFence);

    CARAPI_(void) Remove(
        /* [in] */ IPendingIntent* intent);

    CARAPI_(void) Remove(
        /* [in] */ IPendingIntent* intent,
        /* [in] */ Boolean localOnly);

    CARAPI_(Int32) GetNumbOfGeoFences();

    CARAPI_(AutoPtr<ICollection>) GetAllGeoFences();

    CARAPI_(AutoPtr<IGeoFenceParams>) GetGeoFence(
        /* [in] */ IPendingIntent* intent);

    CARAPI_(Boolean) HasCaller(
        /* [in] */ Int32 uid);

    virtual CARAPI_(void) RemoveCaller(
        /* [in] */ Int32 uid);

    CARAPI_(void) TransferService(
        /* [in] */ GeoFencerBase* geofencer);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

protected:
    virtual CARAPI_(Boolean) Start(
        /* [in] */ IGeoFenceParams* geoFence) = 0;

    virtual CARAPI_(Boolean) Stop(
        /* [in] */ IPendingIntent* intent) = 0;

private:
    static const String TAG;
    AutoPtr<IHashMap> mGeoFences;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GEOFENCERBASE_H__

