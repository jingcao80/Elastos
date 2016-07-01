#ifndef __ELASTOS_DROID_DIALER_LIST_CALLCONTACTSFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_LIST_CALLCONTACTSFRAGMENT_H__

#include "_Elastos_Droid_Dialer_List_CAllContactsFragment.h"
#include "elastos/apps/dialer/list/AllContactsFragment.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CALLCONTACTSFRAGMENT_H__
