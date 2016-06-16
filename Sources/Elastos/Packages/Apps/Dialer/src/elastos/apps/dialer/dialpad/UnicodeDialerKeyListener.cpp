
#include "dialpad/UnicodeDialerKeyListener.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Dialpad {

CAR_INTERFACE_IMPL(UnicodeDialerKeyListener, DialerKeyListener, IUnicodeDialerKeyListener);

AutoPtr<IUnicodeDialerKeyListener> CreateInstance()
{
    AutoPtr<IUnicodeDialerKeyListener> listener;
    CUnicodeDialerKeyListener::New((IUnicodeDialerKeyListener**)&listener);
    return listener;
}

const AutoPtr<IUnicodeDialerKeyListener> UnicodeDialerKeyListener::INSTANCE = CreateInstance();

ECode UnicodeDialerKeyListener::Filter(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISpanned* dest,
    /* [in] */ Int32 dstart,
    /* [in] */ Int32 dend,
    /* [out] */ ICharSequence** ret)
{
    String str;
    source->ToString(&str);
    String digits;
    PhoneNumberUtils::ReplaceUnicodeDigits(str, &digits);
    String converted;
    PhoneNumberUtils::ConvertKeypadLettersToDigits(digits, &converted);
    // PhoneNumberUtils.replaceUnicodeDigits performs a character for character replacement,
    // so we can assume that start and end positions should remain unchanged.
    AutoPtr<ICharSequence> result;
    DialerKeyListener::Filter(converted, start,
            end, dest, dstart, dend, (ICharSequence**)&result);
    if (result == NULL) {
        if (source.Equals(converted)) {
            // There was no conversion or filtering performed. Just return null according to
            // the behavior of DialerKeyListener.
            *ret = NULL;
            return NOERROR;
        } else {
            // filter returns null if the charsequence is to be returned unchanged/unfiltered.
            // But in this case we do want to return a modified character string (even if
            // none of the characters in the modified string are filtered). So if
            // result == null we return the unfiltered but converted numeric string instead.
            *ret = CoreUtils::Convert(converted.SubString(start, end));
            REFCOUNT_ADD(*ret);
            return NOERROR;
        }
    }
    *ret = result;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

} // Dialpad
} // Dialer
} // Apps
} // Elastos
