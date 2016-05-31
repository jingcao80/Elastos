#ifndef __ELASTOS_APPS_DIALER_LIST_CSMARTDIALSEARCHFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_CSMARTDIALSEARCHFRAGMENT_H__

#include "_Elastos_Apps_Dialer_List_CSmartDialSearchFragment.h"
#include "SmartDialSearchFragment.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

CarClass(CSmartDialSearchFragment)
    , public SmartDialSearchFragment
{
public:
    CAR_OBJECT_DECL()
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CSMARTDIALSEARCHFRAGMENT_H__
