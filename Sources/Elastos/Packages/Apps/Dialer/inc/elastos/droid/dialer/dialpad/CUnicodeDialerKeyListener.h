#ifndef __ELASTOS_DROID_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENER_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENER_H__

#include "_Elastos_Droid_Dialer_Dialpad_CUnicodeDialerKeyListener.h"
#include "elastos/apps/dialer/dialpad/UnicodeDialerKeyListener.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_CUNICODEDIALERKEYLISTENER_H__