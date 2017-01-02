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

#include "elastos/droid/location/CAddress.h"
#include "elastos/droid/location/CCountry.h"
#include "elastos/droid/location/CCountryDetector.h"
#include "elastos/droid/location/CCriteria.h"
#include "elastos/droid/location/CFusedBatchOptions.h"
#include "elastos/droid/location/CGeoFenceParams.h"
#include "elastos/droid/location/CGeocoder.h"
#include "elastos/droid/location/CGeocoderParams.h"
#include "elastos/droid/location/CGpsClock.h"
#include "elastos/droid/location/CGpsMeasurement.h"
#include "elastos/droid/location/CGpsSatellite.h"
#include "elastos/droid/location/CGpsStatus.h"
#include "elastos/droid/location/CLocation.h"
#include "elastos/droid/location/CLocationProvider.h"
#include "elastos/droid/location/CLocationRequest.h"
#include "elastos/droid/location/CGpsMeasurementListenerTransport.h"
#include "elastos/droid/location/CGpsMeasurementListenerTransportListenerTransport.h"
#include "elastos/droid/location/CGpsMeasurementsEvent.h"
#include "elastos/droid/location/CGpsNavigationMessageListenerTransport.h"
#include "elastos/droid/location/CGpsNavigationMessageListenerTransportListenerTransport.h"
#include "elastos/droid/location/CGpsNavigationMessage.h"
#include "elastos/droid/location/CGpsNavigationMessageEvent.h"
#include "elastos/droid/location/CLocationManager.h"
#include "elastos/droid/location/CLocationManagerListenerTransport.h"
#include "elastos/droid/location/CLocationManagerGpsStatusListenerTransport.h"

namespace Elastos {
namespace Droid {
namespace Location {

CAR_OBJECT_IMPL(CAddress)
CAR_OBJECT_IMPL(CCountry)
CAR_OBJECT_IMPL(CCountryDetector)

CAR_OBJECT_IMPL(CCriteria)
CAR_OBJECT_IMPL(CFusedBatchOptions)
CAR_OBJECT_IMPL(CGeoFenceParams)
CAR_OBJECT_IMPL(CGeocoder)

CAR_OBJECT_IMPL(CGeocoderParams)
CAR_OBJECT_IMPL(CGpsClock)
CAR_OBJECT_IMPL(CGpsMeasurement)

CAR_OBJECT_IMPL(CGpsSatellite)
CAR_OBJECT_IMPL(CGpsStatus)
CAR_OBJECT_IMPL(CLocation)

CAR_OBJECT_IMPL(CLocationProvider)
CAR_OBJECT_IMPL(CLocationRequest)
CAR_OBJECT_IMPL(CGpsMeasurementListenerTransport)

CAR_OBJECT_IMPL(CGpsMeasurementListenerTransportListenerTransport)
CAR_OBJECT_IMPL(CGpsMeasurementsEvent)
CAR_OBJECT_IMPL(CGpsNavigationMessageListenerTransport)

CAR_OBJECT_IMPL(CGpsNavigationMessageListenerTransportListenerTransport)
CAR_OBJECT_IMPL(CGpsNavigationMessage)
CAR_OBJECT_IMPL(CGpsNavigationMessageEvent)

CAR_OBJECT_IMPL(CLocationManager)
CAR_OBJECT_IMPL(CLocationManagerListenerTransport)
CAR_OBJECT_IMPL(CLocationManagerGpsStatusListenerTransport)
} // Location
} // Droid
} // Elastos