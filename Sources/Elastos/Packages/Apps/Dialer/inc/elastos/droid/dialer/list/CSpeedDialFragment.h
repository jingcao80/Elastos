#ifndef __ELASTOS_DROID_DIALER_LIST_CSPEEDDIALFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_LIST_CSPEEDDIALFRAGMENT_H__

#include "_Elastos_Droid_Dialer_List_CSpeedDialFragment.h"
#include "elastos/apps/dialer/list/SpeedDialFragment.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CSPEEDDIALFRAGMENT_H__
