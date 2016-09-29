
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/dialer/calllog/PhoneNumberDisplayHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

PhoneNumberDisplayHelper::PhoneNumberDisplayHelper(
    /* [in] */ IResources* resources)
{
    mResources = resources;
    mPhoneNumberUtils = new PhoneNumberUtilsWrapper();
}

PhoneNumberDisplayHelper::PhoneNumberDisplayHelper(
    /* [in] */ PhoneNumberUtilsWrapper* phoneNumberUtils,
    /* [in] */ IResources* resources)
{
    mPhoneNumberUtils = phoneNumberUtils;
    mResources = resources;
}

AutoPtr<ICharSequence> PhoneNumberDisplayHelper::GetDisplayName(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 presentation)
{
    if (presentation == ICalls::PRESENTATION_UNKNOWN) {
        String str;
        mResources->GetString(Elastos::Droid::Dialer::R::string::unknown, &str);
        return CoreUtils::Convert(str);
    }
    if (presentation == ICalls::PRESENTATION_RESTRICTED) {
        String str;
        mResources->GetString(Elastos::Droid::Dialer::R::string::private_num, &str);
        return CoreUtils::Convert(str);
    }
    if (presentation == ICalls::PRESENTATION_PAYPHONE) {
        String str;
        mResources->GetString(Elastos::Droid::Dialer::R::string::payphone, &str);
        return CoreUtils::Convert(str);
    }

    Boolean result;
    if (mPhoneNumberUtils->IsVoicemailNumber(number, &result), result) {
        String str;
        mResources->GetString(Elastos::Droid::Dialer::R::string::voicemail, &str);
        return CoreUtils::Convert(str);
    }
    if (PhoneNumberUtilsWrapper::IsLegacyUnknownNumbers(number)) {
        String str;
        mResources->GetString(Elastos::Droid::Dialer::R::string::unknown, &str);
        return CoreUtils::Convert(str);
    }

    return CoreUtils::Convert(String(""));
}

AutoPtr<ICharSequence> PhoneNumberDisplayHelper::GetDisplayNumber(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 presentation,
    /* [in] */ ICharSequence* formattedNumber)
{
    AutoPtr<ICharSequence> displayName = GetDisplayName(number, presentation);
    if (!TextUtils::IsEmpty(displayName)) {
        return displayName;
    }

    if (TextUtils::IsEmpty(number)) {
        AutoPtr<ICharSequence> cs = CoreUtils::Convert(String(""));
        return cs;
    }

    if (TextUtils::IsEmpty(formattedNumber)) {
        return number;
    }
    else {
        return formattedNumber;
    }
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
