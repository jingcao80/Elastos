
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSYNTHETICSMSMESSAGEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSYNTHETICSMSMESSAGEHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CSyntheticSmsMessageHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSyntheticSmsMessageHelper)
    , public Singleton
    , public ISyntheticSmsMessageHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsSyntheticPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ Boolean* result);

    CARAPI CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ ISyntheticSmsMessage** result);
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSYNTHETICSMSMESSAGEHELPER_H__
