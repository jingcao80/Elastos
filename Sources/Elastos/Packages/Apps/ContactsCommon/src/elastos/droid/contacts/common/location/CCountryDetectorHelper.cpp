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

#include "CountryDetector.h"
#include "CCountryDetectorHelper.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

CAR_INTERFACE_IMPL(CCountryDetectorHelper, Singleton, ICountryDetectorHelper)

CAR_SINGLETON_IMPL(CCountryDetectorHelper)

ECode CCountryDetectorHelper::RegisterForLocationUpdates(
    /* [in] */ IContext* context,
    /* [in] */ ILocationManager* locationManager)
{
    CountryDetector::RegisterForLocationUpdates(context, locationManager);
    return NOERROR;
}

ECode CCountryDetectorHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ ICountryDetector** detector)
{
    VALUE_NOT_NULL(detector);
    AutoPtr<ICountryDetector> cd = CountryDetector::GetInstance(context);
    *detector = cd;
    REFCOUNT_ADD(*detector)
    return NOERROR;
}

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos
