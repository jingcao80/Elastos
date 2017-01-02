//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

const AutoPtr<PhoneNumberUtilsWrapper> PhoneNumberUtilsWrapper::INSTANCE = new PhoneNumberUtilsWrapper();

static AutoPtr< Set<String> > CreateLEGACY_UNKNOWN_NUMBERS()
{
    AutoPtr< Set<String> > numbers = new Set<String>();\
    numbers->Insert(String("-1"));
    numbers->Insert(String("-2"));
    numbers->Insert(String("-3"));
    return numbers;
}

const AutoPtr< Set<String> > PhoneNumberUtilsWrapper::LEGACY_UNKNOWN_NUMBERS = CreateLEGACY_UNKNOWN_NUMBERS();

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
    return number != NULL && (LEGACY_UNKNOWN_NUMBERS->Find(str) != LEGACY_UNKNOWN_NUMBERS->End());
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
