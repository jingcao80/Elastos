
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CPHONENUMBERDISPLAYHELPER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CPHONENUMBERDISPLAYHELPER_H__

#include "_Elastos_Apps_Dialer_CallLog_CPhoneNumberDisplayHelper.h"

using Elastos::Droid::Telephony::IPhoneNumberUtils;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

CarClass(CPhoneNumberDisplayHelper)
    , public Object
    , public IPhoneNumberDisplayHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IResources* resources);

    CARAPI constructor(
        /* [in] */ IPhoneNumberUtilsWrapper* phoneNumberUtils,
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
    CARAPI GetDisplayNumber(
        /* [in] */ ICharSequence* number,
        /* [in] */ Int32 presentation,
        /* [in] */ ICharSequence* formattedNumber,
        /* [out] */ ICharSequence** result);

private:
    AutoPtr<IPhoneNumberUtilsWrapper> mPhoneNumberUtils;
    AutoPtr<IResources> mResources;
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CPHONENUMBERDISPLAYHELPER_H__
