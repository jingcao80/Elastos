#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CUSERDATAHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CUSERDATAHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_Sms_CUserDataHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CarClass(CUserDataHelper)
    , public Singleton
    , public IUserDataHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI StringToAscii(
        /* [in] */ const String& str,
        /* [out, callee] */ ArrayOf<Byte>** result);

};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CUSERDATAHELPER_H__
