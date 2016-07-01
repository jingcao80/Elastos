#ifndef __ELASTOS_DROID_DIALER_LIST_CSMARTDIALNUMBERLISTADAPTER_H__
#define __ELASTOS_DROID_DIALER_LIST_CSMARTDIALNUMBERLISTADAPTER_H__

#include "_Elastos_Droid_Dialer_List_CSmartDialNumberListAdapter.h"
#include "elastos/apps/dialer/list/SmartDialNumberListAdapter.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CarClass(CSmartDialNumberListAdapter)
    , public SmartDialNumberListAdapter
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CSMARTDIALNUMBERLISTADAPTER_H__
