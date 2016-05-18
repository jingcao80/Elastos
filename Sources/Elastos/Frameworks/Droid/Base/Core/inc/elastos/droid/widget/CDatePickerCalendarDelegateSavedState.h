#ifndef __ELASTOS_DROID_WIDGET_CDATEPICKERCALENDARDELEGATESAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CDATEPICKERCALENDARDELEGATESAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CDatePickerCalendarDelegateSavedState.h"
#include "elastos/droid/widget/DatePickerCalendarDelegate.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CDatePickerCalendarDelegateSavedState)
    , public DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_CDATEPICKERCALENDARDELEGATESAVEDSTATE_H__
