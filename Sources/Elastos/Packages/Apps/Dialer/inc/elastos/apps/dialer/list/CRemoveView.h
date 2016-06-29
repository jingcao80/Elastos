#ifndef __ELASTOS_APPS_DIALER_LIST_CREMOVEVIEW_H__
#define __ELASTOS_APPS_DIALER_LIST_CREMOVEVIEW_H__

#include "_Elastos_Apps_Dialer_List_CRemoveView.h"
#include "elastos/apps/dialer/list/RemoveView.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

CarClass(CRemoveView)
    , public RemoveView
{
public:
    CAR_OBJECT_DECL();
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CREMOVEVIEW_H__
