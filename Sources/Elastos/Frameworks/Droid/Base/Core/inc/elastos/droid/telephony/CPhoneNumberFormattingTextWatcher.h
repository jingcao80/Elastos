
#ifndef __ELASTOS_DROID_TELEPHONY_CPHONENUMBERFORMATTINGTEXTWATCHER_H__
#define __ELASTOS_DROID_TELEPHONY_CPHONENUMBERFORMATTINGTEXTWATCHER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Telephony_CPhoneNumberFormattingTextWatcher.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::INoCopySpan;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CPhoneNumberFormattingTextWatcher)
    , public Object
    , public IPhoneNumberFormattingTextWatcher
    , public ITextWatcher
    , public INoCopySpan
{
public:
    CPhoneNumberFormattingTextWatcher();

    virtual ~CPhoneNumberFormattingTextWatcher();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& countryCode);

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

private:

    CARAPI_(String) Reformat(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 cursor);

    CARAPI_(String) GetFormattedNumber(
        /* [in] */ Char32 lastNonSeparator,
        /* [in] */ Boolean hasCursor);

    CARAPI_(void) StopFormatting();

    CARAPI_(Boolean) HasSeparator(
        /* [in] */ ICharSequence* s,
        /* [in] */ const Int32 start,
        /* [in] */ const Int32 count);

    /**
     * Indicates the change was caused by ourselves.
     */
    Boolean mSelfChange;

    /**
     * Indicates the formatting has been stopped.
     */
    Boolean mStopFormatting;

// TODO: Need AsYouTypeFormatter
    //AutoPtr<AsYouTypeFormatter> mFormatter;
};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CPHONENUMBERFORMATTINGTEXTWATCHER_H__
