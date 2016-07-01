
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CPHONENUMBERDISPLAYHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CPHONENUMBERDISPLAYHELPER_H__

#include "_Elastos_Droid_Dialer_CallLog_CPhoneNumberDisplayHelper.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telephony.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CarClass(CPhoneNumberDisplayHelper)
    , public Object
    , public IPhoneNumberDisplayHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CPHONENUMBERDISPLAYHELPER_H__
