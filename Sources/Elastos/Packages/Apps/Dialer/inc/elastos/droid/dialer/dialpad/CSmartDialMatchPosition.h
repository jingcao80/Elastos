#ifndef __ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALMATCHPOSITION_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALMATCHPOSITION_H__

#include "_Elastos_Droid_Dialer_Dialpad_CSmartDialMatchPosition.h"
#include "elastos/apps/dialer/dialpad/SmartDialMatchPosition.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CarClass(CSmartDialMatchPosition)
    , public SmartDialMatchPosition
{
public:
    CAR_OBJECT_DECL();
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALMATCHPOSITION_H__
