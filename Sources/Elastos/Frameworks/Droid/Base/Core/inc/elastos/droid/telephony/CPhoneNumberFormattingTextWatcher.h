
#ifndef __ELASTOS_DROID_TELEPHONY_CPHONENUMBERFORMATTINGTEXTWATCHER_H__
#define __ELASTOS_DROID_TELEPHONY_CPHONENUMBERFORMATTINGTEXTWATCHER_H__

#include "_Elastos_Droid_Telephony_CPhoneNumberFormattingTextWatcher.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::Text::IEditable;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CPhoneNumberFormattingTextWatcher)
{
public:
    CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 count);

    CARAPI AfterTextChanged(
        /* [in] */ IEditable* s);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& countryCode);

private:

    CARAPI Init(
        /* [in] */ const String& countryCode);

    CARAPI_(String) Reformat(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 cursor);

    CARAPI_(String) GetFormattedNumber(
        /* [in] */ Char32 lastNonSeparator,
        /* [in] */ Boolean hasCursor);

    CARAPI StopFormatting();

    CARAPI_(Boolean) HasSeparator(
        /* [in] */ ICharSequence* s,
        /* [in] */ const Int32 start,
        /* [in] */ const Int32 count);


    /**
     * Indicates the change was caused by ourselves.
     */
    Boolean mSelfChange/* = FALSE*/;

    /**
     * Indicates the formatting has been stopped.
     */
    Boolean mStopFormatting;

    //AutoPtr<AsYouTypeFormatter> mFormatter;
};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CPHONENUMBERFORMATTINGTEXTWATCHER_H__
