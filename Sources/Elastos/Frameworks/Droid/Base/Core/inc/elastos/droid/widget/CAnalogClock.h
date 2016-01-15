
#ifndef __ELASTOS_DROID_WIDGET_CANALOGCLOCK_H__
#define __ELASTOS_DROID_WIDGET_CANALOGCLOCK_H__

#include "_Elastos_Droid_Widget_CAnalogClock.h"
#include "elastos/droid/widget/AnalogClock.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CAnalogClock)
    , public AnalogClock
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CANALOGCLOCK_H__

