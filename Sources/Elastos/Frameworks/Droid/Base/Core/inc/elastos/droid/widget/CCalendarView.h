
#ifndef __ELASTOS_DROID_WIDGET_CCALENDARVIEW_H__
#define __ELASTOS_DROID_WIDGET_CCALENDARVIEW_H__

#include "_Elastos_Droid_Widget_CCalendarView.h"
#include "elastos/droid/widget/CalendarView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CCalendarView)
    , public CalendarView
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CCALENDARVIEW_H__
