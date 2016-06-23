
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCELLBROADCASTHANDLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCELLBROADCASTHANDLERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmCellBroadcastHandlerHelper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhoneBase;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * Handler for 3GPP format Cell Broadcasts. Parent class can also handle CDMA Cell Broadcasts.
 */
CarClass(CGsmCellBroadcastHandlerHelper)
    , public Singleton
    , public IGsmCellBroadcastHandlerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

   CARAPI MakeGsmCellBroadcastHandler(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneBase* phone,
        /* [out] */ IGsmCellBroadcastHandler** result);

};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCELLBROADCASTHANDLERHELPER_H__
