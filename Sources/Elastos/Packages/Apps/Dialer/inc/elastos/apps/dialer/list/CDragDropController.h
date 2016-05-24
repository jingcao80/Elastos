#ifndef __ELASTOS_APPS_DIALER_LIST_CDRAGDROPCONTROLLER_H__
#define __ELASTOS_APPS_DIALER_LIST_CDRAGDROPCONTROLLER_H__

#include "_Elastos_Apps_Dialer_List_CDragDropController.h"
#include "DragDropController.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

CarClass(CDragDropController)
    , public DragDropController
{
public:
    CAR_OBJECT_DECL()
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_CDRAGDROPCONTROLLER_H__
