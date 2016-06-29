#ifndef __ELASTOS_APPS_DIALER_LIST_CREGULARSEARCHLISTADAPTER_H__
#define __ELASTOS_APPS_DIALER_LIST_CREGULARSEARCHLISTADAPTER_H__

#include "_Elastos_Apps_Dialer_List_CRegularSearchListAdapter.h"
#include "elastos/apps/dialer/list/RegularSearchListAdapter.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

CarClass(CRegularSearchListAdapter)
    , public RegularSearchListAdapter
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CREGULARSEARCHLISTADAPTER_H__
