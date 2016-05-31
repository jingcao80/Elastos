#ifndef __ELASTOS_APPS_DIALER_LIST_CSEARCHFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_CSEARCHFRAGMENT_H__

#include "_Elastos_Apps_Dialer_List_CSearchFragment.h"
#include "SearchFragment.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

CarClass(CSearchFragment)
    , public SearchFragment
{
public:
    CAR_OBJECT_DECL()
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CSEARCHFRAGMENT_H__
