
#include "UpdateCountryService.h"

using Elastos::Droid::Location::IGeocoder;
using Elastos::Droid::Location::CGeocoder;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

const String UpdateCountryService::TAG("UpdateCountryService");

const String UpdateCountryService::ACTION_UPDATE_COUNTRY("saveCountry");

const String UpdateCountryService::KEY_INTENT_LOCATION("location");

CAR_INTERFACE_IMPL(UpdateCountryService, IntentService, IUpdateCountryService)

ECode UpdateCountryService::constructor()
{
    return IntentService::constructor(TAG);
}

void UpdateCountryService::UpdateCountry(
    /* [in] */ IContext* context,
    /* [in] */ ILocation* location)
{
    AutoPtr<IIntent> serviceIntent;
    CIntent::New(context, ECLSID_CUpdateCountryService, (IIntent**)&serviceIntent);
    serviceIntent->SetAction(ACTION_UPDATE_COUNTRY);
    serviceIntent->PutExtra(KEY_INTENT_LOCATION, location);
    context->StartService(serviceIntent);
}

ECode UpdateCountryService::OnHandleIntent(
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (ACTION_UPDATE_COUNTRY.Equals(action)) {
        AutoPtr<IParcelable> value;
        intent->GetParcelableExtra(KEY_INTENT_LOCATION, (IParcelable**)&value);
        ILocation* location = ILocation::Probe(value);

        AutoPtr<IContext> context;
        GetApplicationContext((IContext**)&context);
        String country = GetCountryFromLocation(context, location);

        if (country.IsNull()) {
            return NOERROR;
        }

        AutoPtr<IPreferenceManagerHelper> helper;
        CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&helper);
        AutoPtr<ISharedPreferences> prefs;
        helper->GetDefaultSharedPreferences(context, (ISharedPreferences**)&prefs);

        AutoPtr<IEditor> editor;
        prefs->Edit((IEditor**)&editor);

        AutoPtr<ISystem> systemObj;
        CSystem::AcquireSingleton((ISystem**)&systemObj);
        Int64 timeMillis;
        systemObj->GetCurrentTimeMillis(&timeMillis);
        editor->PutInt64(ICountryDetector::KEY_PREFERENCE_TIME_UPDATED, timeMillis);
        editor->PutString(ICountryDetector::KEY_PREFERENCE_CURRENT_COUNTRY, country);
        editor->Commit();
    }

    return NOERROR;
}

String UpdateCountryService::GetCountryFromLocation(
    /* [in] */ IContext* context,
    /* [in] */ ILocation* location)
{
    AutoPtr<IGeocoder> geocoder;
    CGeocoder::New(context, (IGeocoder**)&geocoder);

    String country;
    // try {
    Double latitude, longitude;
    location->GetLatitude(&latitude);
    location->GetLongitude(&longitude);
    AutoPtr<IList> addresses;
    geocoder->GetFromLocation(latitude, longitude, 1, (IList**)&addresses);

    final List<Address> addresses = geocoder.getFromLocation(
            location.getLatitude(), location.getLongitude(), 1);
    Int32 size;
    if (addresses != NULL && addresses->GetSize(&size), size > 0) {
        AutoPtr<IInterface> item;
        addresses->Get(0, (IInterface**)&item);
        IAddress::Probe(item)->GetCountryCode(&country);
    }
    // } catch (IOException e) {
    //     Log.w(TAG, "Exception occurred when getting geocoded country from location");
    // }
    return country;
}

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos
