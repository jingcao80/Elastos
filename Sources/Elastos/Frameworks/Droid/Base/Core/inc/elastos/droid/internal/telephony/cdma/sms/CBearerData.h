#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CBEARERDATA_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CBEARERDATA_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_Sms_CBearerData.h"
#include "elastos/droid/internal/telephony/cdma/sms/BearerData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CarClass(CBearerData)
    , public BearerData
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

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CBEARERDATA_H__
