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

#include "GeoUtil.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

String GeoUtil::GetCurrentCountryIso(
    /* [in] */ IContext* context)
{
    // The {@link CountryDetector} should never return null so this is safe to return as-is.
    String result;
    CountryDetector::GetInstance(context)->GetCurrentCountryIso(&result);
    return result;
}

String GeoUtil::GetGeocodedLocationFor(
    /* [in] */ IContext* context,
    /* [in] */ const String& phoneNumber)
{
    assert(0 && "TODO");
    // AutoPtr<IPhoneNumberOfflineGeocoder> geocoder = PhoneNumberOfflineGeocoder::GetInstance();
    // AutoPtr<IPhoneNumberUtil> phoneNumberUtil = PhoneNumberUtil::GetInstance();
    // // try {
    // final Phonenumber.PhoneNumber structuredPhoneNumber =
    //         phoneNumberUtil.parse(phoneNumber, getCurrentCountryIso(context));
    // final Locale locale = context.getResources().getConfiguration().locale;
    // return geocoder.getDescriptionForNumber(structuredPhoneNumber, locale);
    // } catch (NumberParseException e) {
    //     return null;
    // }
}

} // Common
} // Contacts
} // Apps
} // Elastos
