#ifndef __ELASTOS_DROID_DIALER_LIST_CREGULARSEARCHFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_LIST_CREGULARSEARCHFRAGMENT_H__

#include "_Elastos_Droid_Dialer_List_CRegularSearchFragment.h"
#include "elastos/apps/dialer/list/RegularSearchFragment.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CREGULARSEARCHFRAGMENT_H__
