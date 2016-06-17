
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CPROXYCONTROLLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CPROXYCONTROLLERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CProxyControllerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CProxyControllerHelper)
    , public Singleton
    , public IProxyControllerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<IPhone*>* phoneProxy,
        /* [in] */ IUiccController* uiccController,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci,
        /* [out] */ IProxyController** result);

    CARAPI GetInstance(
        /* [out] */ IProxyController** result);
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CPROXYCONTROLLERHELPER_H__
