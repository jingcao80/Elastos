
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_PHONENUMBERDISPLAYHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_PHONENUMBERDISPLAYHELPER_H__

#include "elastos/droid/dialer/calllog/PhoneNumberUtilsWrapper.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

class PhoneNumberDisplayHelper : public Object
{
public:
    PhoneNumberDisplayHelper(
        /* [in] */ IResources* resources);

    PhoneNumberDisplayHelper(
        /* [in] */ PhoneNumberUtilsWrapper* phoneNumberUtils,
        /* [in] */ IResources* resources);

    CARAPI_(AutoPtr<ICharSequence>) GetDisplayName(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 presentation);

    /**
     * Returns the string to display for the given phone number.
     *
     * @param number the number to display
     * @param formattedNumber the formatted number if available, may be null
     */
    CARAPI_(AutoPtr<ICharSequence>) GetDisplayNumber(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 presentation,
        /* [in] */ ICharSequence* formattedNumber);

private:
    AutoPtr<PhoneNumberUtilsWrapper> mPhoneNumberUtils;
    AutoPtr<IResources> mResources;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_PHONENUMBERDISPLAYHELPER_H__
