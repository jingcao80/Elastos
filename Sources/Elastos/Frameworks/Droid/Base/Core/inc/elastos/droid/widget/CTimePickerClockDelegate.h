
#ifndef __ELASTOS_DROID_WIDGET_CTIMEPICKERCLOCKDELEGATE_H__
#define __ELASTOS_DROID_WIDGET_CTIMEPICKERCLOCKDELEGATE_H__

#include "_Elastos_Droid_Widget_CTimePickerClockDelegate.h"
#include "elastos/droid/widget/TimePickerClockDelegate.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CTimePickerClockDelegate)
    , public TimePickerClockDelegate
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CTIMEPICKERCLOCKDELEGATE_H__

