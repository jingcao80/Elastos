#ifndef __ELASTOS_APPS_DIALER_LIST_CREGULARSEARCHFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_CREGULARSEARCHFRAGMENT_H__

#include "_Elastos_Apps_Dialer_List_CRegularSearchFragment.h"
#include "elastos/apps/dialer/list/RegularSearchFragment.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

CarClass(CRegularSearchFragment)
    , public RegularSearchFragment
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CREGULARSEARCHFRAGMENT_H__
