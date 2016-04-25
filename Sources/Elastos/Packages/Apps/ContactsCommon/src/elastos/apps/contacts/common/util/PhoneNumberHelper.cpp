
#include "PhoneNumberHelper.h"
#include <elastos/core/Character.h>

using Elastos::Core::Character;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

Boolean PhoneNumberHelper::IsUriNumber(
    /* [in] */ const String& number)
{
    // Note we allow either "@" or "%40" to indicate a URI, in case
    // the passed-in string is URI-escaped.  (Neither "@" nor "%40"
    // will ever be found in a legal PSTN number.)
    return number.IsNull() && (number.Contains("@") || number.Contains("%40"));
}

String PhoneNumberHelper::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& phoneNumberE164,
    /* [in] */ const String& defaultCountryIso)
{
    assert(0 && "TODO");

    Int32 len = phoneNumber->GetLength();
    // for (Int32 i = 0; i < len; i++) {
    //     if (!PhoneNumberUtils::IsDialable(phoneNumber->CharAt(i))) {
    //         return phoneNumber;
    //     }
    // }
    // AutoPtr<IPhoneNumberUtil> util = PhoneNumberUtil::GetInstance();
    // // Get the country code from phoneNumberE164
    // if (!phoneNumberE164.IsNull() && phoneNumberE164->GetLength() >= 2
    //         && phoneNumberE164->CharAt(0) == '+') {
    //     // try {
    //     // The number to be parsed is in E164 format, so the default region used doesn't
    //     // matter.
    //     AutoPtr<IPhoneNumber> pn;
    //     util->Parse(phoneNumberE164, String("ZZ"), (IPhoneNumber**)&pn);
    //     String regionCode;
    //     util->GetRegionCodeForNumber(pn, &regionCode);
    //     if (!TextUtils::IsEmpty(regionCode) &&
    //             // This makes sure phoneNumber doesn't contain an IDD
    //             NormalizeNumber(phoneNumber).IndexOf(phoneNumberE164.Substring(1)) <= 0) {
    //         defaultCountryIso = regionCode;
    //     }
    //     // } catch (NumberParseException e) {
    //     //     Log.w(LOG_TAG, "The number could not be parsed in E164 format!");
    //     // }
    // }

    String result = FormatNumber(phoneNumber, defaultCountryIso);
    return result.IsNull() ? phoneNumber : result;
}


String PhoneNumberHelper::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& defaultCountryIso)
{
    // Do not attempt to format numbers that start with a hash or star symbol.
    if (phoneNumber.StartsWith("#") || phoneNumber.StartsWith("*")) {
        return phoneNumber;
    }

    assert(0 && "TODO");
    // AutoPtr<IPhoneNumberUtil> util = PhoneNumberUtil::GetInstance();
    // String result;
    // // try {
    // AutoPtr<IPhoneNumber> pn;
    // util->ParseAndKeepRawInput(phoneNumber, defaultCountryIso, (IPhoneNumber**)&pn);
    // util->FormatInOriginalFormat(pn, defaultCountryIso, &result);
    // // } catch (NumberParseException e) {
    // //     Log.w(LOG_TAG, "Number could not be parsed with the given country code!");
    // // }
    // return result;
}

String PhoneNumberHelper::NormalizeNumber(
    /* [in] */ const String& phoneNumber)
{
    AutoPtr<StringBuilder> sb = new StringBuilder();
    Int32 len = phoneNumber->GetLength();
    for (Int32 i = 0; i < len; i++) {
        Char32 c = phoneNumber->GetChar(i);
        // Character.digit() supports ASCII and Unicode digits (fullwidth, Arabic-Indic, etc.)
        Int32 digit = Character::ToDigit(c, 10);
        if (digit != -1) {
            sb->Append(digit);
        }
        else if (i == 0 && c == '+') {
            sb->Append(c);
        }
        else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            // return NormalizeNumber(PhoneNumberUtils::ConvertKeypadLettersToDigits(phoneNumber));
        }
    }
    return sb->ToString();
}

String PhoneNumberHelper::GetUsernameFromUriNumber(
    /* [in] */ const String& phoneNumber)
{
    // The delimiter between username and domain name can be
    // either "@" or "%40" (the URI-escaped equivalent.)
    int delimiterIndex = number.indexOf('@');
    if (delimiterIndex < 0) {
        delimiterIndex = number.indexOf("%40");
    }
    if (delimiterIndex < 0) {
        Log.w(LOG_TAG,
                "getUsernameFromUriNumber: no delimiter found in SIP addr '" + number + "'");
        return number;
    }
    return number.substring(0, delimiterIndex);
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
