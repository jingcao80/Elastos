#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CCDMASMSADDRESS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CCDMASMSADDRESS_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_Sms_CCdmaSmsAddress.h"
#include "elastos/droid/internal/telephony/cdma/sms/CdmaSmsAddress.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CarClass(CCdmaSmsAddress)
    , public CdmaSmsAddress
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

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CCDMASMSADDRESS_H__
