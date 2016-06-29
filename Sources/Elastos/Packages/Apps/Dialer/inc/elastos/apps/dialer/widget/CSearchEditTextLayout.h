#ifndef __ELASTOS_APPS_DIALER_WIDGET_CSEARCHEDITTEXTLAYOUT_H__
#define __ELASTOS_APPS_DIALER_WIDGET_CSEARCHEDITTEXTLAYOUT_H__

#include "_Elastos_Apps_Dialer_Widget_CSearchEditTextLayout.h"
#include "elastos/apps/dialer/widget/SearchEditTextLayout.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Widget {

CarClass(CSearchEditTextLayout)
    , public SearchEditTextLayout
{
public:
    CAR_OBJECT_DECL();
};

} // Widget
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_WIDGET_CSEARCHEDITTEXTLAYOUT_H__
