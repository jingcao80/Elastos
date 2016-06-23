
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMINBOUNDSMSHANDLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMINBOUNDSMSHANDLERHELPER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmInboundSmsHandlerHelper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::ISmsStorageMonitor;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CarClass(CGsmInboundSmsHandlerHelper)
    , public Singleton
    , public IGsmInboundSmsHandlerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI MakeInboundSmsHandler(
        /* [in] */ IContext* context,
        /* [in] */ ISmsStorageMonitor* storageMonitor,
        /* [in] */ IPhoneBase* phone,
        /* [out] */ IGsmInboundSmsHandler** result);
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMINBOUNDSMSHANDLERHELPER_H__
