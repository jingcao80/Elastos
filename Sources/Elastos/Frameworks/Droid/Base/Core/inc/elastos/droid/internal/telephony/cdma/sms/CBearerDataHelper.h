#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CBEARERDATAHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CBEARERDATAHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_Sms_CBearerDataHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CarClass(CBearerDataHelper)
    , public Singleton
    , public IBearerDataHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI CalcTextEncodingDetails(
        /* [in] */ ICharSequence* msg,
        /* [in] */ Boolean force7BitEncoding,
        /* [out] */ IGsmAlphabetTextEncodingDetails** result);

    CARAPI Encode(
        /* [in] */ IBearerData* bData,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* smsData,
        /* [out] */ IBearerData** result);

    CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* smsData,
        /* [in] */ Int32 serviceCategory,
        /* [out] */ IBearerData** result);
};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CBEARERDATAHELPER_H__
