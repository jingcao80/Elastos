#ifndef __ELASTOS_DROID_WIDGET_CDIGITALCLOCK_H__
#define __ELASTOS_DROID_WIDGET_CDIGITALCLOCK_H__

#include "_Elastos_Droid_Widget_CDigitalClock.h"
#include "elastos/droid/widget/DigitalClock.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CDigitalClock)
    , public DigitalClock
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CDIGITALCLOCK_H__
