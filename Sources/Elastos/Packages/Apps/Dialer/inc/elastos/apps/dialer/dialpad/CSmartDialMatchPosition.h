#ifndef __ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALMATCHPOSITION_H__
#define __ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALMATCHPOSITION_H__

#include "_Elastos_Apps_Dialer_Dialpad_CSmartDialMatchPosition.h"
#include "SmartDialMatchPosition.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Dialpad {

CarClass(CSmartDialMatchPosition)
    , public SmartDialMatchPosition
{
public:
    CAR_OBJECT_DECL()
};

} // Dialpad
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALMATCHPOSITION_H__
