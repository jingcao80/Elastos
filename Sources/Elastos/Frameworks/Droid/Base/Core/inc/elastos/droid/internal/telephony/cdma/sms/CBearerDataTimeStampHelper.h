#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CBEARERDATATIMESTAMPHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CBEARERDATATIMESTAMPHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_Sms_CBearerDataTimeStampHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CarClass(CBearerDataTimeStampHelper)
    , public Singleton
    , public IBearerDataTimeStampHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI FromByteArray(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IBearerDataTimeStamp** result);
};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CBEARERDATATIMESTAMPHELPER_H__
