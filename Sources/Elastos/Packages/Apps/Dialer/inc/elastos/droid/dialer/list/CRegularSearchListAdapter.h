#ifndef __ELASTOS_DROID_DIALER_LIST_CREGULARSEARCHLISTADAPTER_H__
#define __ELASTOS_DROID_DIALER_LIST_CREGULARSEARCHLISTADAPTER_H__

#include "_Elastos_Droid_Dialer_List_CRegularSearchListAdapter.h"
#include "elastos/apps/dialer/list/RegularSearchListAdapter.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CREGULARSEARCHLISTADAPTER_H__
