
#include "elastos/apps/dialer/calllog/CPhoneNumberDisplayHelper.h"
#include "elastos/apps/dialer/calllog/CPhoneNumberUtilsWrapper.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/droid/text/TextUtils.h>
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CPhoneNumberDisplayHelper, Object, IPhoneNumberDisplayHelper);

CAR_OBJECT_IMPL(CPhoneNumberDisplayHelper);

ECode CPhoneNumberDisplayHelper::constructor(
    /* [in] */ IResources* resources)
{
    mResources = resources;
    return CPhoneNumberUtilsWrapper::New(
            (IPhoneNumberUtilsWrapper**)&mPhoneNumberUtils);
}

ECode CPhoneNumberDisplayHelper::constructor(
    /* [in] */ IPhoneNumberUtilsWrapper* phoneNumberUtils,
    /* [in] */ IResources* resources)
{
    mPhoneNumberUtils = phoneNumberUtils;
    mResources = resources;
    return NOERROR;
}

AutoPtr<ICharSequence> CPhoneNumberDisplayHelper::GetDisplayName(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 presentation)
{
    if (presentation == ICalls::PRESENTATION_UNKNOWN) {
        String str;
        mResources->GetString(R::string::unknown, &str);
        return CoreUtils::Convert(str);
    }
    if (presentation == ICalls::PRESENTATION_RESTRICTED) {
        String str;
        mResources->GetString(R::string::private_num, &str);
        return CoreUtils::Convert(str);
    }
    if (presentation == ICalls::PRESENTATION_PAYPHONE) {
        String str;
        mResources->GetString(R::string::payphone, &str);
        return CoreUtils::Convert(str);
    }

    Boolean result;
    if (mPhoneNumberUtils->IsVoicemailNumber(number, &result), result) {
        String str;
        mResources->GetString(R::string::voicemail, &str);
        return CoreUtils::Convert(str);
    }
    if (PhoneNumberUtilsWrapper::IsLegacyUnknownNumbers(number)) {
        String str;
        mResources->GetString(R::string::unknown, &str);
        return CoreUtils::Convert(str);
    }

    return CoreUtils::Convert(String(""));
}

ECode CPhoneNumberDisplayHelper::GetDisplayNumber(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 presentation,
    /* [in] */ ICharSequence* formattedNumber,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICharSequence> displayName = GetDisplayName(number, presentation);
    if (!TextUtils::IsEmpty(displayName)) {
        *result = displayName;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if (TextUtils::IsEmpty(number)) {
        AutoPtr<ICharSequence> cs = CoreUtils::Convert(String(""));
        *result = cs;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if (TextUtils::IsEmpty(formattedNumber)) {
        *result = number;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        *result = formattedNumber;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
