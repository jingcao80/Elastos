
#include "elastos/droid/dialer/calllog/PhoneNumberUtilsWrapper.h"
#include "elastos/droid/contacts/common/util/PhoneNumberHelper.h"
#include <elastos/droid/text/TextUtils.h>
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"

using Elastos::Droid::Contacts::Common::Util::PhoneNumberHelper;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

static AutoPtr<PhoneNumberUtilsWrapper> CreateINSTANCE()
{
    return new PhoneNumberUtilsWrapper();
}
const AutoPtr<PhoneNumberUtilsWrapper> PhoneNumberUtilsWrapper::INSTANCE = CreateINSTANCE();

static Set<String>& CreateLEGACY_UNKNOWN_NUMBERS()
{
    Set<String> numbers;
    numbers.Insert(String("-1"));
    numbers.Insert(String("-2"));
    numbers.Insert(String("-3"));
    return numbers;
}
const Set<String> PhoneNumberUtilsWrapper::LEGACY_UNKNOWN_NUMBERS = CreateLEGACY_UNKNOWN_NUMBERS();

Boolean PhoneNumberUtilsWrapper::CanPlaceCallsTo(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 presentation)
{
    return presentation == ICalls::PRESENTATION_ALLOWED
            && !TextUtils::IsEmpty(number) && !IsLegacyUnknownNumbers(number);
}

Boolean PhoneNumberUtilsWrapper::CanSendSmsTo(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 presentation)
{
    return CanPlaceCallsTo(number, presentation) && !IsVoicemailNumber(number) && !IsSipNumber(
            number);
}

Boolean PhoneNumberUtilsWrapper::IsVoicemailNumber(
    /* [in] */ ICharSequence* number)
{
    if (number == NULL) {
        return FALSE;
    }
    else {
        AutoPtr<IPhoneNumberUtils> utils;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
        String str;
        number->ToString(&str);
        Boolean result;
        utils->IsVoiceMailNumber(str, &result);
        return result;
    }
}

Boolean PhoneNumberUtilsWrapper::IsSipNumber(
    /* [in] */ ICharSequence* number)
{
    if (number == NULL) {
        return FALSE;
    }
    else {
        String str;
        number->ToString(&str);
        return PhoneNumberHelper::IsUriNumber(str);
    }
}

Boolean PhoneNumberUtilsWrapper::IsUnknownNumberThatCanBeLookedUp(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 presentation)
{
    if (presentation == ICalls::PRESENTATION_UNKNOWN) {
        return FALSE;
    }
    if (presentation == ICalls::PRESENTATION_RESTRICTED) {
        return FALSE;
    }
    if (presentation == ICalls::PRESENTATION_PAYPHONE) {
        return FALSE;
    }
    if (TextUtils::IsEmpty(number)) {
        return FALSE;
    }
    if (INSTANCE->IsVoicemailNumber(number)) {
        return FALSE;
    }
    if (IsLegacyUnknownNumbers(number)) {
        return FALSE;
    }
    return TRUE;
}

Boolean PhoneNumberUtilsWrapper::IsLegacyUnknownNumbers(
    /* [in] */ ICharSequence* number)
{
    String str;
    number->ToString(&str);
    return number != NULL && (LEGACY_UNKNOWN_NUMBERS.Find(str) != LEGACY_UNKNOWN_NUMBERS.End());
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
