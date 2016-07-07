
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
