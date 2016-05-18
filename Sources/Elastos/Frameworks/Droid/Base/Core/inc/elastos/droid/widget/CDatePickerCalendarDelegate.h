#ifndef __ELASTOS_DROID_WIDGET_CDATEPICKERCALENDARDELEGATE_H__
#define __ELASTOS_DROID_WIDGET_CDATEPICKERCALENDARDELEGATE_H__

#include "_Elastos_Droid_Widget_CDatePickerCalendarDelegate.h"
#include "elastos/droid/widget/DatePickerCalendarDelegate.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CDatePickerCalendarDelegate)
    , public DatePickerCalendarDelegate
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_CDATEPICKERCALENDARDELEGATE_H__
