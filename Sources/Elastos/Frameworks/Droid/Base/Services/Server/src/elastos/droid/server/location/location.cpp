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

#include "elastos/droid/server/location/CFusedLocationHardwareService.h"
#include "elastos/droid/server/location/CFusedGeofenceHardwareService.h"
#include "elastos/droid/server/location/CFusedLocationHardwareSecure.h"
#include "elastos/droid/server/location/CGeoFenceListener.h"
#include "elastos/droid/server/location/CGpsStatusProviderService.h"
#include "elastos/droid/server/location/CGpsGeofenceHardwareService.h"
#include "elastos/droid/server/location/CNetInitiatedListener.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CAR_OBJECT_IMPL(CFusedLocationHardwareService)
CAR_OBJECT_IMPL(CFusedGeofenceHardwareService)
CAR_OBJECT_IMPL(CFusedLocationHardwareSecure)
CAR_OBJECT_IMPL(CGeoFenceListener)
CAR_OBJECT_IMPL(CGpsStatusProviderService)
CAR_OBJECT_IMPL(CGpsGeofenceHardwareService)
CAR_OBJECT_IMPL(CNetInitiatedListener)

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos