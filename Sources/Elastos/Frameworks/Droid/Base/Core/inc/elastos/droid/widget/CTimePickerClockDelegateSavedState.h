#ifndef __ELASTOS_DROID_WIDGET_CTIMEPICKERCLOCKDELEGATESAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CTIMEPICKERCLOCKDELEGATESAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CTimePickerClockDelegateSavedState.h"
#include "elastos/droid/widget/TimePickerClockDelegate.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CTimePickerClockDelegateSavedState)
    , public TimePickerClockDelegate::TimePickerClockDelegateSavedState
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_CTIMEPICKERCLOCKDELEGATESAVEDSTATE_H__
