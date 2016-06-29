#ifndef __ELASTOS_APPS_DIALER_WIDGET_CACTIONBARCONTROLLER_H__
#define __ELASTOS_APPS_DIALER_WIDGET_CACTIONBARCONTROLLER_H__

#include "_Elastos_Apps_Dialer_Widget_CActionBarController.h"
#include "elastos/apps/dialer/widget/ActionBarController.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Widget {

CarClass(CActionBarController)
    , public ActionBarController
{
public:
    CAR_OBJECT_DECL();
};

} // Widget
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_WIDGET_CACTIONBARCONTROLLER_H__
