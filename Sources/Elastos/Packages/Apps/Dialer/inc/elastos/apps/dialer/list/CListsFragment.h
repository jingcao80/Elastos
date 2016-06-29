#ifndef __ELASTOS_APPS_DIALER_LIST_CLISTSFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_CLISTSFRAGMENT_H__

#include "_Elastos_Apps_Dialer_List_CListsFragment.h"
#include "elastos/apps/dialer/list/ListsFragment.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

CarClass(CListsFragment)
    , public ListsFragment
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CLISTSFRAGMENT_H__
