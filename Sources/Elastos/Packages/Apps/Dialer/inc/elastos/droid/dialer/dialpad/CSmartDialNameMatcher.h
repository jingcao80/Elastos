
#ifndef __ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALNAMEMATCHER_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALNAMEMATCHER_H__

#include "_Elastos_Droid_Dialer_Dialpad_CSmartDialNameMatcher.h"
#include "elastos/apps/dialer/dialpad/SmartDialNameMatcher.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CarClass(CSmartDialNameMatcher)
    , public SmartDialNameMatcher
{
public:
    CAR_OBJECT_DECL();
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALNAMEMATCHER_H__
