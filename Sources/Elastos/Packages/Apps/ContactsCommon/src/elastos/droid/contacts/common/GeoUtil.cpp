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
