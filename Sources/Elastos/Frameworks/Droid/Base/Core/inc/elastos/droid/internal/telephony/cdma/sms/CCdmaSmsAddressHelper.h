#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CCDMASMSADDRESSHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CCDMASMSADDRESSHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_Sms_CCdmaSmsAddressHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CarClass(CCdmaSmsAddressHelper)
    , public Singleton
    , public ICdmaSmsAddressHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI Parse(
        /* [in] */ const String& address,
        /* [out] */ ICdmaSmsAddress** result);

};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CCDMASMSADDRESSHELPER_H__
