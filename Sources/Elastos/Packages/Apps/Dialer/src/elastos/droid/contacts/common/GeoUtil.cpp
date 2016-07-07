
#include "elastos/droid/contacts/common/GeoUtil.h"

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

String GeoUtil::GetCurrentCountryIso(
    /* [in] */ IContext* context)
{
    // The {@link CountryDetector} should never return null so this is safe to return as-is.
    // TODO:
    return String("CN");
    // return CountryDetector.getInstance(context).getCurrentCountryIso();
}

String GeoUtil::GetGeocodedLocationFor(
    /* [in] */ IContext* context,
    /* [in] */ const String& phoneNumber)
{
    assert(0 && "TODO");
    return String(NULL);
    // final PhoneNumberOfflineGeocoder geocoder = PhoneNumberOfflineGeocoder.getInstance();
    // final PhoneNumberUtil phoneNumberUtil = PhoneNumberUtil.getInstance();
    // try {
    //     final Phonenumber.PhoneNumber structuredPhoneNumber =
    //             phoneNumberUtil.parse(phoneNumber, getCurrentCountryIso(context));
    //     final Locale locale = context.getResources().getConfiguration().locale;
    //     return geocoder.getDescriptionForNumber(structuredPhoneNumber, locale);
    // } catch (NumberParseException e) {
    //     return null;
    // }
}

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
