#ifndef __ELASTOS_DROID_DIALER_WIDGET_CACTIONBARCONTROLLER_H__
#define __ELASTOS_DROID_DIALER_WIDGET_CACTIONBARCONTROLLER_H__

#include "_Elastos_Droid_Dialer_Widget_CActionBarController.h"
#include "elastos/apps/dialer/widget/ActionBarController.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_WIDGET_CACTIONBARCONTROLLER_H__
