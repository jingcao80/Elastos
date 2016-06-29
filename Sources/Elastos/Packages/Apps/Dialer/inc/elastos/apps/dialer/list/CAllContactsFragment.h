#ifndef __ELASTOS_APPS_DIALER_LIST_CALLCONTACTSFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_CALLCONTACTSFRAGMENT_H__

#include "_Elastos_Apps_Dialer_List_CAllContactsFragment.h"
#include "elastos/apps/dialer/list/AllContactsFragment.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

CarClass(CAllContactsFragment)
    , public AllContactsFragment
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CALLCONTACTSFRAGMENT_H__
