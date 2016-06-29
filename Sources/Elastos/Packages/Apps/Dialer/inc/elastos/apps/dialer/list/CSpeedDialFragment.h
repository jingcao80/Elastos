#ifndef __ELASTOS_APPS_DIALER_LIST_CSPEEDDIALFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_CSPEEDDIALFRAGMENT_H__

#include "_Elastos_Apps_Dialer_List_CSpeedDialFragment.h"
#include "elastos/apps/dialer/list/SpeedDialFragment.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

CarClass(CSpeedDialFragment)
    , public SpeedDialFragment
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CSPEEDDIALFRAGMENT_H__
