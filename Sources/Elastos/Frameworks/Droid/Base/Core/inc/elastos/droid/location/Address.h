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

#ifndef __ELASTOS_DROID_LOCATION_ADDRESS_H__
#define __ELASTOS_DROID_LOCATION_ADDRESS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;
using Elastos::Utility::IHashMap;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Location {

class Address
    : public Object
    , public IAddress
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    Address();

    virtual ~Address();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ILocale* locale);

    /**
     * Returns the Locale associated with this address.
     */
    CARAPI GetLocale(
        /* [out] */ ILocale** locale);

    /**
     * Returns the largest index currently in use to specify an address line.
     * If no address lines are specified, -1 is returned.
     */
    CARAPI GetMaxAddressLineIndex(
        /* [out] */ Int32* index);

    /**
     * Returns a line of the address numbered by the given index
     * (starting at 0), or null if no such line is present.
     *
     * @throws IllegalArgumentException if index < 0
     */
    CARAPI GetAddressLine(
        /* [in] */ Int32 index,
        /* [out] */ String* line);

    /**
     * Sets the line of the address numbered by index (starting at 0) to the
     * given String, which may be null.
     *
     * @throws IllegalArgumentException if index < 0
     */
    CARAPI SetAddressLine(
        /* [in] */ Int32 index,
        /* [in] */ const String& line);

    /**
     * Returns the feature name of the address, for example, "Golden Gate Bridge", or null
     * if it is unknown
     */
    CARAPI GetFeatureName(
        /* [out] */ String* featureName);

    /**
     * Sets the feature name of the address to the given String, which may be null
     */
    CARAPI SetFeatureName(
        /* [in] */ const String& featureName);

    /**
     * Returns the administrative area name of the address, for example, "CA", or null if
     * it is unknown
     */
    CARAPI GetAdminArea(
        /* [out] */ String* adminArea);

    /**
     * Sets the administrative area name of the address to the given String, which may be null
     */
    CARAPI SetAdminArea(
        /* [in] */ const String& adminArea);

    /**
     * Returns the sub-administrative area name of the address, for example, "Santa Clara County",
     * or null if it is unknown
     */
    CARAPI GetSubAdminArea(
        /* [out] */ String* subAdminArea);

    /**
     * Sets the sub-administrative area name of the address to the given String, which may be null
     */
    CARAPI SetSubAdminArea(
        /* [in] */ const String& subAdminArea);

    /**
     * Returns the locality of the address, for example "Mountain View", or null if it is unknown.
     */
    CARAPI GetLocality(
        /* [out] */ String* locality);

    /**
     * Sets the locality of the address to the given String, which may be null.
     */
    CARAPI SetLocality(
        /* [in] */ const String& locality);

    /**
     * Returns the sub-locality of the address, or null if it is unknown.
     * For example, this may correspond to the neighborhood of the locality.
     */
    CARAPI GetSubLocality(
        /* [out] */ String* sublocality);

    /**
     * Sets the sub-locality of the address to the given String, which may be null.
     */
    CARAPI SetSubLocality(
        /* [in] */ const String& sublocality);

    /**
     * Returns the thoroughfare name of the address, for example, "1600 Ampitheater Parkway",
     * which may be null
     */
    CARAPI GetThoroughfare(
        /* [out] */ String* thoroughfare);

    /**
     * Sets the thoroughfare name of the address, which may be null.
     */
    CARAPI SetThoroughfare(
        /* [in] */ const String& thoroughfare);

    /**
     * Returns the sub-thoroughfare name of the address, which may be null.
     * This may correspond to the street number of the address.
     */
    CARAPI GetSubThoroughfare(
        /* [out] */ String* subthoroughfare);

    /**
     * Sets the sub-thoroughfare name of the address, which may be null.
     */
    CARAPI SetSubThoroughfare(
        /* [in] */ const String& subthoroughfare);

    /**
     * Returns the premises of the address, or null if it is unknown.
     */
    CARAPI GetPremises(
        /* [out] */ String* premises);

    /**
     * Sets the premises of the address to the given String, which may be null.
     */
    CARAPI SetPremises(
        /* [in] */ const String& premises);

    /**
     * Returns the postal code of the address, for example "94110",
     * or null if it is unknown.
     */
    CARAPI GetPostalCode(
        /* [out] */ String* postalCode);

    /**
     * Sets the postal code of the address to the given String, which may
     * be null.
     */
    CARAPI SetPostalCode(
        /* [in] */ const String& postalCode);

    /**
     * Returns the country code of the address, for example "US",
     * or null if it is unknown.
     */
    CARAPI GetCountryCode(
        /* [out] */ String* countryCode);

    /**
     * Sets the country code of the address to the given String, which may
     * be null.
     */
    CARAPI SetCountryCode(
        /* [in] */ const String& countryCode);

    /**
     * Returns the localized country name of the address, for example "Iceland",
     * or null if it is unknown.
     */
    CARAPI GetCountryName(
        /* [out] */ String* countryName);

    /**
     * Sets the country name of the address to the given String, which may
     * be null.
     */
    CARAPI SetCountryName(
        /* [in] */ const String& countryName);

    /**
     * Returns true if a latitude has been assigned to this Address,
     * false otherwise.
     */
    CARAPI HasLatitude(
        /* [out] */ Boolean* hasLatitude);

    /**
     * Returns the latitude of the address if known.
     *
     * @throws IllegalStateException if this Address has not been assigned
     * a latitude.
     */
    CARAPI GetLatitude(
        /* [out] */ Double* latitude);

    /**
     * Sets the latitude associated with this address.
     */
    CARAPI SetLatitude(
        /* [in] */ Double latitude);

    /**
     * Removes any latitude associated with this address.
     */
    CARAPI ClearLatitude();

    /**
     * Returns true if a longitude has been assigned to this Address,
     * false otherwise.
     */
    CARAPI HasLongitude(
        /* [out] */ Boolean* hasLongitude);

    /**
     * Returns the longitude of the address if known.
     *
     * @throws IllegalStateException if this Address has not been assigned
     * a longitude.
     */
    CARAPI GetLongitude(
        /* [out] */ Double* longitude);

    /**
     * Sets the longitude associated with this address.
     */
    CARAPI SetLongitude(
        /* [in] */ Double longitude);

    /**
     * Removes any longitude associated with this address.
     */
    CARAPI ClearLongitude();

    /**
     * Returns the phone number of the address if known,
     * or null if it is unknown.
     *
     * @throws IllegalStateException if this Address has not been assigned
     * a latitude.
     */
    CARAPI GetPhone(
        /* [out] */ String* phone);

    /**
     * Sets the phone number associated with this address.
     */
    CARAPI SetPhone(
        /* [in] */ const String& phone);

    /**
     * Returns the public URL for the address if known,
     * or null if it is unknown.
     */
    CARAPI GetUrl(
        /* [out] */ String* url);

    /**
     * Sets the public URL associated with this address.
     */
    CARAPI SetUrl(
        /* [in] */ const String& Url);

    /**
     * Returns additional provider-specific information about the
     * address as a Bundle.  The keys and values are determined
     * by the provider.  If no additional information is available,
     * null is returned.
     *
     * <!--
     * <p> A number of common key/value pairs are listed
     * below. Providers that use any of the keys on this list must
     * provide the corresponding value as described below.
     *
     * <ul>
     * </ul>
     * -->
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    /**
     * Sets the extra information associated with this fix to the
     * given Bundle.
     */
    CARAPI SetExtras(
        /* [in] */ IBundle* extras);

    //@Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    AutoPtr<ILocale> mLocale;
    String mFeatureName;
    AutoPtr<IHashMap> mAddressLines;
    Int32 mMaxAddressLineIndex;
    String mAdminArea;
    String mSubAdminArea;
    String mLocality;
    String mSubLocality;
    String mThoroughfare;
    String mSubThoroughfare;
    String mPremises;
    String mPostalCode;
    String mCountryCode;
    String mCountryName;
    Double mLatitude;
    Double mLongitude;
    Boolean mHasLatitude;
    Boolean mHasLongitude;
    String mPhone;
    String mUrl;
    AutoPtr<IBundle> mExtras;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_ADDRESS_H__
