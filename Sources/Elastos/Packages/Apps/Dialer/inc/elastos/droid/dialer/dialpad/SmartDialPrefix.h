
#ifndef __ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALPREFIX_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALPREFIX_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

/**
 * Smart Dial utility class to find prefixes of contacts. It contains both methods to find supported
 * prefix combinations for contact names, and also methods to find supported prefix combinations for
 * contacts' phone numbers. Each contact name is separated into several tokens, such as first name,
 * middle name, family name etc. Each phone number is also separated into country code, NANP area
 * code, and local number if such separation is possible.
 */
class SmartDialPrefix
    : public Object
{
public:
    /**
     * Class to record phone number parsing information.
     */
    class PhoneNumberTokens
        : public Object
        , public ISmartDialPrefixPhoneNumberTokens
    {
    public:
        CAR_INTERFACE_DECL();

        PhoneNumberTokens(
            /* [in] */ const String& countryCode,
            /* [in] */ Int32 countryCodeOffset,
            /* [in] */ Int32 nanpCodeOffset);
    public:
        /** Country code of the phone number. */
        String mCountryCode;

        /** Offset of national number after the country code. */
        Int32 mCountryCodeOffset;

        /** Offset of local number after NANP area code.*/
        Int32 mNanpCodeOffset;
    };

public:
    /** Initializes the Nanp settings, and finds out whether user is in a NANP region.*/
    static CARAPI_(void) InitializeNanpSettings(
        /* [in] */ IContext* context);

    /**
     * Explicitly setting the user Nanp to the given boolean
     */
    // @VisibleForTesting
    static CARAPI_(void) SetUserInNanpRegion(
        /* [in] */ Boolean userInNanpRegion);

    /**
     * Parses a contact's name into a list of separated tokens.
     *
     * @param contactName Contact's name stored in string.
     * @return A list of name tokens, for example separated first names, last name, etc.
     */
    static CARAPI_(AutoPtr<IArrayList>) ParseToIndexTokens(
        /* [in] */ const String& contactName);

    /**
     * Generates a list of strings that any prefix of any string in the list can be used to look
     * up the contact's name.
     *
     * @param index The contact's name in string.
     * @return A List of strings, whose prefix can be used to look up the contact.
     */
    static CARAPI_(AutoPtr<IArrayList>) GenerateNamePrefixes(
        /* [in] */ const String& index);

    /**
     * Computes a list of number strings based on tokens of a given phone number. Any prefix
     * of any string in the list can be used to look up the phone number. The list include the
     * full phone number, the national number if there is a country code in the phone number, and
     * the local number if there is an area code in the phone number following the NANP format.
     * For example, if a user has phone number +41 71 394 8392, the list will contain 41713948392
     * and 713948392. Any prefix to either of the strings can be used to look up the phone number.
     * If a user has a phone number +1 555-302-3029 (NANP format), the list will contain
     * 15553023029, 5553023029, and 3023029.
     *
     * @param number String of user's phone number.
     * @return A list of strings where any prefix of any entry can be used to look up the number.
     */
    static CARAPI_(AutoPtr<IArrayList>) ParseToNumberTokens(
        /* [in] */ const String& number);

    /**
     * Parses a phone number to find out whether it has country code and NANP area code.
     *
     * @param number Raw phone number.
     * @return a PhoneNumberToken instance with country code, NANP code information.
     */
    static CARAPI_(AutoPtr<ISmartDialPrefixPhoneNumberTokens>) ParsePhoneNumber(
        /* [in] */ const String& number);

    static CARAPI_(AutoPtr<ISmartDialMap>) GetMap();

    /**
     * Indicates whether the given country uses NANP numbers
     * @see <a href="https://en.wikipedia.org/wiki/North_American_Numbering_Plan">
     *     https://en.wikipedia.org/wiki/North_American_Numbering_Plan</a>
     *
     * @param country ISO 3166 country code (case doesn't matter)
     * @return True if country uses NANP numbers (e.g. US, Canada), false otherwise
     */
    // @VisibleForTesting
    static CARAPI_(Boolean) IsCountryNanp(
        /* [in] */ const String& country);

    /**
     * Returns whether the user is in a region that uses Nanp format based on the sim location.
     *
     * @return Whether user is in Nanp region.
     */
    static CARAPI_(Boolean) GetUserInNanpRegion();
private:
    /**
     * Checkes whether a country code is valid.
     */
    static CARAPI_(Boolean) IsValidCountryCode(
        /* [in] */ const String& countryCode);

    static CARAPI_(AutoPtr<ISet>) InitCountryCodes();

    static CARAPI_(AutoPtr<ISet>) InitNanpCountries();

private:
    /** The number of starting and ending tokens in a contact's name considered for initials.
     * For example, if both constants are set to 2, and a contact's name is
     * "Albert Ben Charles Daniel Ed Foster", the first two tokens "Albert" "Ben", and last two
     * tokens "Ed" "Foster" can be replaced by their initials in contact name matching.
     * Users can look up this contact by combinations of his initials such as "AF" "BF" "EF" "ABF"
     * "BEF" "ABEF" etc, but can not use combinations such as "CF" "DF" "ACF" "ADF" etc.
     */
    static const Int32 LAST_TOKENS_FOR_INITIALS; // = 2;
    static const Int32 FIRST_TOKENS_FOR_INITIALS; // = 2;

    /** The country code of the user's sim card obtained by calling getSimCountryIso*/
    static const String PREF_USER_SIM_COUNTRY_CODE; // =
            // "DialtactsActivity_user_sim_country_code";
    static const String PREF_USER_SIM_COUNTRY_CODE_DEFAULT; // = null;
    static String sUserSimCountryCode; // = PREF_USER_SIM_COUNTRY_CODE_DEFAULT;

    /** Indicates whether user is in NANP regions.*/
    static Boolean sUserInNanpRegion; // = false;

    /** Set of country names that use NANP code.*/
    static AutoPtr<ISet> sNanpCountries;

    /** Set of supported country codes in front of the phone number. */
    static AutoPtr<ISet> sCountryCodes;

    /** Dialpad mapping. */
    static const AutoPtr<ISmartDialMap> mMap; // = new LatinSmartDialMap();

    static Boolean sNanpInitialized; // = false;
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_SMARTDIALPREFIX_H__
