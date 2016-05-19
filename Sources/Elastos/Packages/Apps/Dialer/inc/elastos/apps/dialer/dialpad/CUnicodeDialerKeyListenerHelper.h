#ifndef __ELASTOS_APPS_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENERHELPER_H__
#define __ELASTOS_APPS_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENERHELPER_H__

#include "_Elastos_Apps_Dialer_Dialpad_CUnicodeDialerKeyListenerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Dialpad {

CarClass(CUnicodeDialerKeyListenerHelper)
    , public Singleton
    , public IUnicodeDialerKeyListenerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetINSTANCE(
        /* [out] */ IUnicodeDialerKeyListener** instance);
};

} // Dialpad
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENERHELPER_H__