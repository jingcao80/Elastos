#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMACONNECTIONHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMACONNECTIONHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_CCdmaConnectionHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaConnectionHelper)
    , public Singleton
    , public ICdmaConnectionHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI FormatDialString(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* result);

};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMACONNECTIONHELPER_H__
