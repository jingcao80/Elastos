#ifndef __ELASTOS_APPS_CONTACTS_COMMON_UTIL_PHONENUMBERHELPER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_UTIL_PHONENUMBERHELPER_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

/**
 * This class wraps several PhoneNumberUtil calls and TelephonyManager calls. Some of them are
 * the same as the ones in the framework's code base. We can remove those once they are part of
 * the public API.
 */
class PhoneNumberHelper
    : public Object
{
public:
    /**
     * Determines if the specified number is actually a URI (i.e. a SIP address) rather than a
     * regular PSTN phone number, based on whether or not the number contains an "@" character.
     *
     * @param number Phone number
     * @return true if number contains @
     *
     * TODO: Remove if PhoneNumberUtils.isUriNumber(String number) is made public.
     */
    static CARAPI_(Boolean) IsUriNumber(
        /* [in] */ const String& number);

    /**
     * Formats the phone number only if the given number hasn't been formatted.
     * <p>
     * The number which has only dailable character is treated as not being
     * formatted.
     *
     * @param phoneNumber the number to be formatted.
     * @param phoneNumberE164 The E164 format number whose country code is used if the given
     * phoneNumber doesn't have the country code.
     * @param defaultCountryIso The ISO 3166-1 two letters country code whose convention will
     * be used if the phoneNumberE164 is null or invalid, or if phoneNumber contains IDD.
     * @return The formatted number if the given number has been formatted, otherwise, return the
     * given number.
     *
     * TODO: Remove if PhoneNumberUtils.formatNumber(String phoneNumber, String phoneNumberE164,
     * String defaultCountryIso) is made public.
     */
    static CARAPI_(String) FormatNumber(
        /* [in] */ const String& phoneNumber,
        /* [in] */ const String& phoneNumberE164,
        /* [in] */ const String& defaultCountryIso);

    /**
     * Format a phone number.
     * <p>
     * If the given number doesn't have the country code, the phone will be
     * formatted to the default country's convention.
     *
     * @param phoneNumber The number to be formatted.
     * @param defaultCountryIso The ISO 3166-1 two letters country code whose convention will
     * be used if the given number doesn't have the country code.
     * @return The formatted number, or null if the given number is not valid.
     *
     * TODO: Remove if PhoneNumberUtils.formatNumber(String phoneNumber, String defaultCountryIso)
     * is made public.
     */
    static CARAPI_(String) FormatNumber(
        /* [in] */ const String& phoneNumber,
        /* [in] */ const String& defaultCountryIso);

    /**
     * Normalize a phone number by removing the characters other than digits. If
     * the given number has keypad letters, the letters will be converted to
     * digits first.
     *
     * @param phoneNumber The number to be normalized.
     * @return The normalized number.
     *
     * TODO: Remove if PhoneNumberUtils.normalizeNumber(String phoneNumber) is made public.
     */
    static CARAPI_(String) NormalizeNumber(
        /* [in] */ const String& phoneNumber);

    /**
     * @return the "username" part of the specified SIP address, i.e. the part before the "@"
     * character (or "%40").
     *
     * @param number SIP address of the form "username@domainname" (or the URI-escaped equivalent
     * "username%40domainname")
     *
     * TODO: Remove if PhoneNumberUtils.getUsernameFromUriNumber(String number) is made public.
     */
    static CARAPI_(String) GetUsernameFromUriNumber(
        /* [in] */ const String& phoneNumber);

private:
    static const String LOG_TAG;

};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_PHONENUMBERHELPER_H__
