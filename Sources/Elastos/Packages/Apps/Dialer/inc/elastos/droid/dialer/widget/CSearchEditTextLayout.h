#ifndef __ELASTOS_DROID_DIALER_WIDGET_CSEARCHEDITTEXTLAYOUT_H__
#define __ELASTOS_DROID_DIALER_WIDGET_CSEARCHEDITTEXTLAYOUT_H__

#include "_Elastos_Droid_Dialer_Widget_CSearchEditTextLayout.h"
#include "elastos/apps/dialer/widget/SearchEditTextLayout.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_WIDGET_CSEARCHEDITTEXTLAYOUT_H__
