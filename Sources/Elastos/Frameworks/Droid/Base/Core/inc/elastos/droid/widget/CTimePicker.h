
#ifndef __ELASTOS_DROID_WIDGET_CTIMEPICKER_H__
#define __ELASTOS_DROID_WIDGET_CTIMEPICKER_H__

#include "_Elastos_Droid_Widget_CTimePicker.h"
#include "elastos/droid/widget/TimePicker.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CTimePicker)
    , public TimePicker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CTIMEPICKER_H__

