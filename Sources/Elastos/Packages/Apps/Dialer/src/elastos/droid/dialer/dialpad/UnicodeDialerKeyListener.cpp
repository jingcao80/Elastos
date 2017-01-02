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

#include "elastos/droid/dialer/dialpad/UnicodeDialerKeyListener.h"
// #include "Elastos.Droid.Telephony.h"
// #include <elastos/core/CoreUtils.h>

// using Elastos::Droid::Telephony::IPhoneNumberUtils;
// using Elastos::Droid::Telephony::CPhoneNumberUtils;
// using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

const AutoPtr<UnicodeDialerKeyListener> UnicodeDialerKeyListener::INSTANCE = new UnicodeDialerKeyListener();

ECode UnicodeDialerKeyListener::Filter(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISpanned* dest,
    /* [in] */ Int32 dstart,
    /* [in] */ Int32 dend,
    /* [out] */ ICharSequence** ret)
{
    // TODO:
    // String str;
    // source->ToString(&str);
    // AutoPtr<IPhoneNumberUtils> utils;
    // CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
    // String digits;
    // utils->ReplaceUnicodeDigits(str, &digits);
    // String converted;
    // utils->ConvertKeypadLettersToDigits(digits, &converted);
    // // PhoneNumberUtils.replaceUnicodeDigits performs a character for character replacement,
    // // so we can assume that start and end positions should remain unchanged.
    // AutoPtr<ICharSequence> result;
    // assert(0 && "TODO");
    // // DialerKeyListener::Filter(converted, start,
    // //         end, dest, dstart, dend, (ICharSequence**)&result);
    // if (result == NULL) {
    //     if (str.Equals(converted)) {
    //         // There was no conversion or filtering performed. Just return null according to
    //         // the behavior of DialerKeyListener.
    //         *ret = NULL;
    //         return NOERROR;
    //     }
    //     else {
    //         // filter returns null if the charsequence is to be returned unchanged/unfiltered.
    //         // But in this case we do want to return a modified character string (even if
    //         // none of the characters in the modified string are filtered). So if
    //         // result == null we return the unfiltered but converted numeric string instead.
    //         *ret = CoreUtils::Convert(converted.Substring(start, end));
    //         REFCOUNT_ADD(*ret);
    //         return NOERROR;
    //     }
    // }
    // *ret = result;
    // REFCOUNT_ADD(*ret);
    return NOERROR;
}

} // Dialpad
} // Dialer
} // Droid
} // Elastos
