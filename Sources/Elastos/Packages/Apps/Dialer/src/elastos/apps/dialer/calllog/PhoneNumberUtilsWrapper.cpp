
#include "elastos/apps/dialer/calllog/PhoneNumberUtilsWrapper.h"

using Elastos ::Droid:Telephony::IPhoneNumberUtils;
using Elastos ::Droid:Telephony::CPhoneNumberUtils;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

AutoPtr<IPhoneNumberUtilsWrapper> CreateINSTANCE()
{
    AutoPtr<IPhoneNumberUtilsWrapper> wrapper;
    CPhoneNumberUtilsWrapper::New((IPhoneNumberUtilsWrapper**)&wrapper);
    return wrapper;
}

AutoPtr<ISet> CreateLEGACY_UNKNOWN_NUMBERS()
{
    assert(0 && "TODO");
}

AutoPtr<IPhoneNumberUtilsWrapper> PhoneNumberUtilsWrapper::INSTANCE = CreateINSTANCE();

AutoPtr<ISet> PhoneNumberUtilsWrapper::LEGACY_UNKNOWN_NUMBERS = CreateLEGACY_UNKNOWN_NUMBERS();

CAR_INTERFACE_IMPL(PhoneNumberUtilsWrapper, Object, IPhoneNumberUtilsWrapper);

Boolean PhoneNumberUtilsWrapper::CanPlaceCallsTo(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 presentation)
{
    return presentation == ICalls::PRESENTATION_ALLOWED
            && !TextUtils::IsEmpty(number) && !IsLegacyUnknownNumbers(number);
}

ECode PhoneNumberUtilsWrapper::CanSendSmsTo(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 presentation,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    Boolean isVoicemailNumber;
    Boolean isSipNumber;
    *result = CanPlaceCallsTo(number, presentation)
            && IsVoicemailNumber(number, &isVoicemailNumber), !isVoicemailNumber
            && IsSipNumber(number, &isSipNumber), !isSipNumber;
    return NOERROR;
}

ECode PhoneNumberUtilsWrapper::IsVoicemailNumber(
    /* [in] */ ICharSequence* number,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    if (number == NULL) {
        *result = FALSE;
    }
    else {
        AutoPtr<IPhoneNumberUtils> utils;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
        String str;
        number->ToString(&str);
        utils->IsVoiceMailNumber(result);
    }
    return NOERROR;
}

ECode PhoneNumberUtilsWrapper::IsSipNumber(
    /* [in] */ ICharSequence* number,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    if (number == NULL) {
        *result = FALSE;
    }
    else {
        String str;
        number->ToString(&str);
        *result = PhoneNumberHelper::IsUriNumber(str);
    }
    return NOERROR;
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
    Boolean isVoicemailNumber;
    if (INSTANCE->IsVoicemailNumber(number, &isVoicemailNumber), isVoicemailNumber) {
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
    Boolean contains;
    return number != NULL && LEGACY_UNKNOWN_NUMBERS->Contains(number, &contains), contains;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
