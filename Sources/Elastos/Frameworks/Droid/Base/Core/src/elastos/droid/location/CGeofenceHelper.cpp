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

#include "elastos/droid/location/CGeofenceHelper.h"
#include "elastos/droid/location/Geofence.h"

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL(CGeofenceHelper, Singleton, IGeofenceHelper)

CAR_SINGLETON_IMPL(CGeofenceHelper)

ECode CGeofenceHelper::CreateCircle(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Float radius,
    /* [out] */ IGeofence** geofence)
{
    VALIDATE_NOT_NULL(geofence)
    return Geofence::CreateCircle(latitude, longitude, radius, geofence);
}

} // namespace Location
} // namespace Droid
} // namespace Elastos