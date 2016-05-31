#ifndef __ELASTOS_APPS_DIALER_LIST_CSMARTDIALNUMBERLISTADAPTER_H__
#define __ELASTOS_APPS_DIALER_LIST_CSMARTDIALNUMBERLISTADAPTER_H__

#include "_Elastos_Apps_Dialer_List_CSmartDialNumberListAdapter.h"
#include "SmartDialNumberListAdapter.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

CarClass(CSmartDialNumberListAdapter)
    , public SmartDialNumberListAdapter
{
public:
    CAR_OBJECT_DECL()
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CSMARTDIALNUMBERLISTADAPTER_H__
