#ifndef __ELASTOS_APPS_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENER_H__
#define __ELASTOS_APPS_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENER_H__

#include "_Elastos_Apps_Dialer_Dialpad_CUnicodeDialerKeyListener.h"
#include "elastos/apps/dialer/dialpad/UnicodeDialerKeyListener.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Dialpad {

CarClass(CUnicodeDialerKeyListener)
    , public UnicodeDialerKeyListener
{
public:
    CAR_OBJECT_DECL();
};

} // Dialpad
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENER_H__