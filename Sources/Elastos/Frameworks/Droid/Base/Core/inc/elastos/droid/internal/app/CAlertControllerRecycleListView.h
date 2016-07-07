#ifndef __ELASTOS_DROID_INTERNAL_APP_CALERTCONTROLLERRECYCLELISTVIEW_H__
#define __ELASTOS_DROID_INTERNAL_APP_CALERTCONTROLLERRECYCLELISTVIEW_H__

#include "_Elastos_Droid_Internal_App_CAlertControllerRecycleListView.h"
#include "elastos/droid/internal/app/CAlertController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CAlertControllerRecycleListView)
    , public CAlertController::RecycleListView
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CALERTCONTROLLERRECYCLELISTVIEW_H__
