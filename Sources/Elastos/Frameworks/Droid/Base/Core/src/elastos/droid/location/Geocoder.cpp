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

#include "elastos/droid/location/Geocoder.h"
#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/location/CGeocoderParams.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::ServiceManager;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

const String Geocoder::TAG("Geocoder");

CAR_INTERFACE_IMPL(Geocoder, Object, IGeocoder);

Geocoder::Geocoder()
{}

ECode Geocoder::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ILocale* locale)
{
    if (locale == NULL) {
        Logger::E(TAG, "locale == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    CGeocoderParams::New(context, locale, (IGeocoderParams**)&mParams);
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::LOCATION_SERVICE);
    mService = IILocationManager::Probe(obj);
    return NOERROR;
}

ECode Geocoder::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);
    return constructor(context, locale);
}

ECode Geocoder::GetFromLocation(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Int32 maxResults,
    /* [out] */ IList** addressContainer)
{
    VALIDATE_NOT_NULL(addressContainer);

    if (latitude < -90.0 || latitude > 90.0) {
        Logger::E(TAG, "latitude == %d" ,latitude);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (longitude < -180.0 || longitude > 180.0) {
        Logger::E(TAG, "longitude == %d" ,longitude);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String ex;
    mService->GetFromLocation(latitude, longitude, maxResults,
        mParams, addressContainer, &ex);
    if (!ex.IsNull()) {
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode Geocoder::GetFromLocationName(
    /* [in] */ const String& locationName,
    /* [in] */ Int32 maxResults,
    /* [out] */ IList** addressContainer)
{
    if (locationName == NULL) {
        Logger::E(TAG, "locationName == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String ex;
    ECode ec = mService->GetFromLocationName(locationName,
        0, 0, 0, 0, maxResults, mParams, addressContainer, &ex);
    if (FAILED(ec)) {
        Logger::E(TAG, "getFromLocationName: got RemoteException%08x", ec);
        *addressContainer = NULL;
        return E_REMOTE_EXCEPTION;
    }
    if (!ex.IsNull()) {
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode Geocoder::GetFromLocationName(
    /* [in] */ const String& locationName,
    /* [in] */ Int32 maxResults,
    /* [in] */ Double lowerLeftLatitude,
    /* [in] */ Double lowerLeftLongitude,
    /* [in] */ Double upperRightLatitude,
    /* [in] */ Double upperRightLongitude,
    /* [out] */ IList** addressContainer)
{
    if (locationName == NULL) {
        Logger::E(TAG, "locationName == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (lowerLeftLatitude < -90.0 || lowerLeftLatitude > 90.0) {
        Logger::E(TAG, "lowerLeftLatitude == %d", lowerLeftLatitude);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (lowerLeftLongitude < -180.0 || lowerLeftLongitude > 180.0) {
        Logger::E(TAG, "lowerLeftLongitude == %d", lowerLeftLongitude);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (upperRightLatitude < -90.0 || upperRightLatitude > 90.0) {
        Logger::E(TAG, "upperRightLatitude == %d", upperRightLatitude);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (upperRightLongitude < -180.0 || upperRightLongitude > 180.0) {
        Logger::E(TAG, "upperRightLongitude == %d", upperRightLongitude);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String ex;
    ECode ec = mService->GetFromLocationName(locationName,
        lowerLeftLatitude, lowerLeftLongitude, upperRightLatitude, upperRightLongitude,
        maxResults, mParams, addressContainer, &ex);
    if (FAILED(ec)) {
        Logger::E(TAG, "getFromLocationName: got RemoteException%08x", ec);
        *addressContainer = NULL;
        return E_REMOTE_EXCEPTION;
    }
    if (!ex.IsNull()) {
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
