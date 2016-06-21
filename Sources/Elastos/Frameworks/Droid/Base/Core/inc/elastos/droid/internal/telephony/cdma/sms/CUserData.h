#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CUSERDATA_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CUSERDATA_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_Sms_CUserData.h"
#include "elastos/droid/internal/telephony/cdma/sms/UserData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CarClass(CUserData)
    , public UserData
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CUSERDATA_H__
