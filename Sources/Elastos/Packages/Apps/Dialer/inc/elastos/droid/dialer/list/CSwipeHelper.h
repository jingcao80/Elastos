#ifndef __ELASTOS_DROID_DIALER_LIST_CSWIPEHELPER_H__
#define __ELASTOS_DROID_DIALER_LIST_CSWIPEHELPER_H__

#include "_Elastos_Droid_Dialer_List_CSwipeHelper.h"
#include "elastos/apps/dialer/list/SwipeHelper.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CarClass(CSwipeHelper)
    , public SwipeHelper
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CSWIPEHELPER_H__
