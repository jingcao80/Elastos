#ifndef __ELASTOS_DROID_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENERHELPER_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENERHELPER_H__

#include "_Elastos_Droid_Dialer_Dialpad_CUnicodeDialerKeyListenerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CarClass(CUnicodeDialerKeyListenerHelper)
    , public Singleton
    , public IUnicodeDialerKeyListenerHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI GetINSTANCE(
        /* [out] */ IUnicodeDialerKeyListener** instance);
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENERHELPER_H__