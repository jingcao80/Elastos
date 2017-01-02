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

#ifndef __ELASTOS_DROID_LOCATION_GEOCODERPARAMS_H__
#define __ELASTOS_DROID_LOCATION_GEOCODERPARAMS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Location {

 /**
 * This class contains extra parameters to pass to an IGeocodeProvider
 * implementation from the Geocoder class.  Currently this contains the
 * language, country and variant information from the Geocoder's locale
 * as well as the Geocoder client's package name for geocoder server
 * logging.  This information is kept in a separate class to allow for
 * future expansion of the IGeocodeProvider interface.
 *
 * @hide
 */
class GeocoderParams
    : public Object
    , public IGeocoderParams
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    GeocoderParams();

    CARAPI constructor();

    /**
     * This object is only constructed by the Geocoder class
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ILocale* locale);

    /**
     * returns the Geocoder's locale
     */
    CARAPI GetLocale(
        /* [out] */ ILocale** locale);

    /**
     * returns the package name of the Geocoder's client
     */
    CARAPI GetClientPackage(
        /* [out] */ String* packageName);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

private:
    AutoPtr<ILocale> mLocale;
    String mPackageName;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_GEOCODERPARAMS_H__
