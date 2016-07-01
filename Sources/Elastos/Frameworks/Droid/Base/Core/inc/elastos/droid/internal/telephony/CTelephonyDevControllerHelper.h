
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CTELEPHONYDEVCONTROLLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CTELEPHONYDEVCONTROLLERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CTelephonyDevControllerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CTelephonyDevControllerHelper)
    , public Singleton
    , public ITelephonyDevControllerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Create(
        /* [out] */ ITelephonyDevController** result);

    CARAPI GetInstance(
        /* [out] */ ITelephonyDevController** result);

    /**
     * each RIL call this interface to register/unregister the unsolicited hardware
     * configuration callback data it can provide.
     */
    CARAPI RegisterRIL(
        /* [in] */ ICommandsInterface* cmdsIf);

    CARAPI UnregisterRIL(
        /* [in] */ ICommandsInterface* cmdsIf);

    /**
     * get total number of registered modem.
     */
    CARAPI GetModemCount(
        /* [out] */ Int32* result);
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CTELEPHONYDEVCONTROLLERHELPER_H__
