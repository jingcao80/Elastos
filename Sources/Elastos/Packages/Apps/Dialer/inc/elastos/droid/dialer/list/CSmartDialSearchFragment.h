#ifndef __ELASTOS_DROID_DIALER_LIST_CSMARTDIALSEARCHFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_LIST_CSMARTDIALSEARCHFRAGMENT_H__

#include "_Elastos_Droid_Dialer_List_CSmartDialSearchFragment.h"
#include "elastos/apps/dialer/list/SmartDialSearchFragment.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CarClass(CSmartDialSearchFragment)
    , public SmartDialSearchFragment
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CSMARTDIALSEARCHFRAGMENT_H__
