
#ifndef __ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALNAMEMATCHER_H__
#define __ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALNAMEMATCHER_H__

#include "_Elastos_Apps_Dialer_Dialpad_CSmartDialNameMatcher.h"
#include "SmartDialNameMatcher.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Dialpad {

CarClass(CSmartDialNameMatcher)
    , public SmartDialNameMatcher
{
public:
    CAR_OBJECT_DECL()
};

} // Dialpad
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALNAMEMATCHER_H__
