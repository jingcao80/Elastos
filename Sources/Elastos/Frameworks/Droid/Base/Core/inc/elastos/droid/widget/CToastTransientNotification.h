#ifndef __ELASTOS_DROID_WIDGET_CTOASTTRANSIENTNOTIFICATION_H__
#define __ELASTOS_DROID_WIDGET_CTOASTTRANSIENTNOTIFICATION_H__

#include "_Elastos_Droid_Widget_CToastTransientNotification.h"
#include "elastos/droid/widget/Toast.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CToastTransientNotification)
    , public Toast::TN
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CTOASTTRANSIENTNOTIFICATION_H__
