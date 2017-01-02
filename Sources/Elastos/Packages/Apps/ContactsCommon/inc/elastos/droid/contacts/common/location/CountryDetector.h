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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_COUNTRYDETECTOR_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_COUNTRYDETECTOR_H__

#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

/**
 * This class is used to detect the country where the user is. It is a simplified version of the
 * country detector service in the framework. The sources of country location are queried in the
 * following order of reliability:
 * <ul>
 * <li>Mobile network</li>
 * <li>Location manager</li>
 * <li>SIM's country</li>
 * <li>User's default locale</li>
 * </ul>
 *
 * As far as possible this class tries to replicate the behavior of the system's country detector
 * service:
 * 1) Order in priority of sources of country location
 * 2) Mobile network information provided by CDMA phones is ignored
 * 3) Location information is updated every 12 hours (instead of 24 hours in the system)
 * 4) Location updates only uses the {@link LocationManager#PASSIVE_PROVIDER} to avoid active use
 *    of the GPS
 * 5) If a location is successfully obtained and geocoded, we never fall back to use of the
 *    SIM's country (for the system, the fallback never happens without a reboot)
 * 6) Location is not used if the device does not implement a {@link android.location.Geocoder}
*/
class CountryDetector
    : public Object
    , public ICountryDetector
{
public:
    /**
     * Class that can be used to return the user's default locale. This is in its own class so that
     * it can be mocked out.
     */
    class LocaleProvider
        : public Object
        , public ICountryDetectorLocaleProvider
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetDefaultLocale(
            /* [out] */ ILocale** locale);

    };

    class LocationChangedReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CountryDetector::LocationChangedReceiver")

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    };

public:
    static CARAPI_(void) RegisterForLocationUpdates(
        /* [in] */ IContext* context,
        /* [in] */ ILocationManager* locationManager);

    /**
     * Factory method for {@link CountryDetector} that allows the caller to provide mock objects.
     */
    // @NeededForTesting
    CARAPI_(AutoPtr<ICountryDetector>) GetInstanceForTest(
        /* [in] */ IContext* context,
        /* [in] */ ITelephonyManager* telephonyManager,
        /* [in] */ ILocationManager* locationManager,
        /* [in] */ ILocaleProvider* localeProvider,
        /* [in] */ IGeocoder* geocoder);

    /**
     * Returns the instance of the country detector. {@link #initialize(Context)} must have been
     * called previously.
     *
     * @return the initialized country detector.
     */
    static CARAPI_(AutoPtr<ICountryDetector>) GetInstance(
        /* [in] */ IContext* context);

    CARAPI GetCurrentCountryIso(
        /* [out] */ String* iso);

private:
    CountryDetector(
        /* [in] */ IContext* context);

    CountryDetector(
        /* [in] */ IContext* context,
        /* [in] */ ITelephonyManager* telephonyManager,
        /* [in] */ ILocationManager* locationManager,
        /* [in] */ ILocaleProvider* localeProvider);

    /**
     * @return the country code of the current telephony network the user is connected to.
     */
    CARAPI_(String) GetNetworkBasedCountryIso();

    /**
     * @return the geocoded country code detected by the {@link LocationManager}.
     */
    CARAPI_(String) GetLocationBasedCountryIso();

    /**
     * @return the country code of the SIM card currently inserted in the device.
     */
    CARAPI_(String) GetSimBasedCountryIso();

    /**
     * @return the country code of the user's currently selected locale.
     */
    CARAPI_(String) GetLocaleBasedCountryIso();

    CARAPI_(Boolean) IsNetworkCountryCodeAvailable();

private:
    static const String TAG; // = "CountryDetector";

    static AutoPtr<ICountryDetector> sInstance;

    AutoPtr<ITelephonyManager> mTelephonyManager;
    AutoPtr<ILocationManager> mLocationManager;
    AutoPtr<ICountryDetectorLocaleProvider> mLocaleProvider;

    // Used as a default country code when all the sources of country data have failed in the
    // exceedingly rare event that the device does not have a default locale set for some reason.
    String DEFAULT_COUNTRY_ISO; // = "US";

    // Wait 12 hours between updates
    static const Int64 TIME_BETWEEN_UPDATES_MS; // = 1000L * 60 * 60 * 12;

    // Minimum distance before an update is triggered, in meters. We don't need this to be too
    // exact because all we care about is what country the user is in.
    static const Int64 DISTANCE_BETWEEN_UPDATES_METERS; // = 5000;
    AutoPtr<IContext> mContext;
};

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LOCATION_COUNTRYDETECTOR_H__
