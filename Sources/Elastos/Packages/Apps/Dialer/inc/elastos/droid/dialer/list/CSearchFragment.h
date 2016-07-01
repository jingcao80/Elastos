#ifndef __ELASTOS_DROID_DIALER_LIST_CSEARCHFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_LIST_CSEARCHFRAGMENT_H__

#include "_Elastos_Droid_Dialer_List_CSearchFragment.h"
#include "elastos/apps/dialer/list/SearchFragment.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CarClass(CSearchFragment)
    , public SearchFragment
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CSEARCHFRAGMENT_H__
