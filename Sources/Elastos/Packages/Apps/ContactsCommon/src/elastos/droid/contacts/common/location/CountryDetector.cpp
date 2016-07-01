
#include "CountryDetector.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Location::IGeocoderHelper;
using Elastos::Droid::Location::CGeocoderHelper;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

const String CountryDetector::TAG("CountryDetector");

AutoPtr<ICountryDetector> CountryDetector::sInstance;

const Int64 CountryDetector::TIME_BETWEEN_UPDATES_MS = 1000LL * 60 * 60 * 12;

const Int64 CountryDetector::DISTANCE_BETWEEN_UPDATES_METERS = 5000;

//=================================================================
// CountryDetector::LocaleProvider
//=================================================================
CAR_INTERFACE_IMPL(CountryDetector::LocaleProvider, Object, ICountryDetectorLocaleProvider)

ECode CountryDetector::LocaleProvider::GetDefaultLocale(
    /* [out] */ ILocale** locale)
{
    VALUE_NOT_NULL(locale);

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> defaultLocale;
    helper->GetDefault((ILocale**)&defaultLocale);
    *locale = defaultLocale;
    REFCOUNT_ADD(*locale);

    return NOERROR;
}

//=================================================================
// CountryDetector::LocationChangedReceiver
//=================================================================
ECode CountryDetector::LocationChangedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Boolean hasExtra = FALSE;
    if (intent->HasExtra(ILocationManager::KEY_LOCATION_CHANGED, &hasExtra), !hasExtra) {
        return NOERROR;
    }

    AutoPtr<IBundle> bundle;
    intent->GetExtras((IBundle**)&bundle);
    AutoPtr<IParcelable> parcelable;
    bundle->GetParcelable(ILocationManager::KEY_LOCATION_CHANGED, (IParcelable**)&parcelable);
    ILocation* location = ILocation::Probe(parcelable);

    UpdateCountryService::UpdateCountry(context, location);

    return NOERROR;
}

//=================================================================
// CountryDetector
//=================================================================
CAR_INTERFACE_IMPL(CountryDetector, Object, ICountryDetector)

CountryDetector::CountryDetector(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> telephony;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&telephony);
    AutoPtr<IInterface> locationManager;
    context->GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&locationManager);
    AutoPtr<LocaleProvider> provider = new LocaleProvider();
    CountryDetector(context, telephony,
            locationManager, (ICountryDetectorLocaleProvider*)provider);
}

CountryDetector::CountryDetector(
    /* [in] */ IContext* context,
    /* [in] */ ITelephonyManager* telephonyManager,
    /* [in] */ ILocationManager* locationManager,
    /* [in] */ ICountryDetectorLocaleProvider* localeProvider)
    : DEFAULT_COUNTRY_ISO("US")
{
    mTelephonyManager = telephonyManager;
    mLocationManager = locationManager;
    mLocaleProvider = localeProvider;
    mContext = context;

    RegisterForLocationUpdates(context, mLocationManager);
}

void CountryDetector::RegisterForLocationUpdates(
    /* [in] */ IContext* context,
    /* [in] */ ILocationManager* locationManager)
{
    AutoPtr<IGeocoderHelper> helper;
    CGeocoderHelper::AcquireSingleton((IGeocoderHelper**)&helper);
    Boolean isPresent = FALSE;
    if (helper->IsPresent(&isPresent), !isPresent) {
        // Certain devices do not have an implementation of a geocoder - in that case there is
        // no point trying to get location updates because we cannot retrieve the country based
        // on the location anyway.
        return;
    }
    AutoPtr<IIntent> activeIntent;
    CIntent::New(context, ECLSID_CLocationChangedReceiver, (IIntent**)&activeIntent);

    AutoPtr<IPendingIntentHelper> piHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
    AutoPtr<IPendingIntent> pendingIntent;
    piHelper->GetBroadcast(context, 0, activeIntent,
            IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&pendingIntent);

    locationManager->RequestLocationUpdates(ILocationManager::PASSIVE_PROVIDER,
            TIME_BETWEEN_UPDATES_MS, DISTANCE_BETWEEN_UPDATES_METERS, pendingIntent);
}

AutoPtr<ICountryDetector> CountryDetector::GetInstanceForTest(
    /* [in] */ IContext* context,
    /* [in] */ ITelephonyManager* telephonyManager,
    /* [in] */ ILocationManager* locationManager,
    /* [in] */ ILocaleProvider* localeProvider,
    /* [in] */ IGeocoder* geocoder)
{
    AutoPtr<CountryDetector> detector = new CountryDetector(
            context, telephonyManager, locationManager, localeProvider);
    return (ICountryDetector*)detector;
}

CARAPI_(AutoPtr<ICountryDetector>) CountryDetector::GetInstance(
    /* [in] */ IContext* context)
{
    {    AutoLock syncLock(this);
        if (sInstance == NULL) {
            AutoPtr<IContext> appContext;
            context->GetApplicationContext((IContext**)&appContext);
            sInstance = (ICountryDetector*)new CountryDetector(appContext);
        }
        return sInstance;
    }
}

ECode CountryDetector::GetCurrentCountryIso(
    /* [out] */ String* iso)
{
    VALUE_NOT_NULL(iso);

    String result;
    if (IsNetworkCountryCodeAvailable()) {
        result = GetNetworkBasedCountryIso();
    }
    if (TextUtils::IsEmpty(result)) {
        result = GetLocationBasedCountryIso();
    }
    if (TextUtils::IsEmpty(result)) {
        result = GetSimBasedCountryIso();
    }
    if (TextUtils::IsEmpty(result)) {
        result = GetLocaleBasedCountryIso();
    }
    if (TextUtils::IsEmpty(result)) {
        result = DEFAULT_COUNTRY_ISO;
    }
    *iso = result.ToUpperCase(ILocale::US);
    return NOERROR;
}

String CountryDetector::GetNetworkBasedCountryIso()
{
    String res;
    mTelephonyManager->GetNetworkCountryIso(&res);
    return res;
}

String CountryDetector::GetLocationBasedCountryIso()
{
    AutoPtr<IGeocoderHelper> helper;
    CGeocoderHelper::AcquireSingleton((IGeocoderHelper**)&helper);
    Boolean isPresent = FALSE;
    if (helper->IsPresent(&isPresent), !isPresent) {
        return String(NULL);
    }

    AutoPtr<IPreferenceManagerHelper> helper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&helper);
    AutoPtr<ISharedPreferences> sharedPreferences;
    helper->GetDefaultSharedPreferences(
            mContext, (ISharedPreferences**)&sharedPreferences);

    String res;
    sharedPreferences->GetString(KEY_PREFERENCE_CURRENT_COUNTRY, String(NULL), &res);
    return res;
}

String CountryDetector::GetSimBasedCountryIso()
{
    String res;
    mTelephonyManager->GetSimCountryIso(&res);
    return res;
}

String CountryDetector::GetLocaleBasedCountryIso()
{
    AutoPtr<ILocale> defaultLocale;
    mLocaleProvider->GetDefaultLocale((ILocale**)&defaultLocale);
    if (defaultLocale != NULL) {
        String res;
        defaultLocale->GetCountry(&res);
        return res;
    }
    return String(NULL);
}

Boolean CountryDetector::IsNetworkCountryCodeAvailable()
{
    // On CDMA TelephonyManager.getNetworkCountryIso() just returns the SIM's country code.
    // In this case, we want to ignore the value returned and fallback to location instead.
    Int32 type;
    mTelephonyManager->GetPhoneType(&type);
    return type == ITelephonyManager::PHONE_TYPE_GSM;
}

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos
