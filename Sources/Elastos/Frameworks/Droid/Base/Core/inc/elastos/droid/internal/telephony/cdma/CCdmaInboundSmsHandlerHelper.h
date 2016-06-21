#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMAINBOUNDSMSHANDLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMAINBOUNDSMSHANDLERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_CCdmaInboundSmsHandlerHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaInboundSmsHandlerHelper)
    , public Singleton
    , public ICdmaInboundSmsHandlerHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI MakeInboundSmsHandler(
        /* [in] */ IContext* context,
        /* [in] */ ISmsStorageMonitor* storageMonitor,
        /* [in] */ IPhoneBase* phone,
        /* [in] */ ICdmaSMSDispatcher* smsDispatcher,
        /* [out] */ ICdmaInboundSmsHandler** result);

};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMAINBOUNDSMSHANDLERHELPER_H__
