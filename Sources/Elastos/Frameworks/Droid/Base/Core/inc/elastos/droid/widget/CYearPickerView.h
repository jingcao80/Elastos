
#ifndef __ELASTOS_DROID_WIDGET_CYEARPICKERVIEW_H__
#define __ELASTOS_DROID_WIDGET_CYEARPICKERVIEW_H__

#include "_Elastos_Droid_Widget_CYearPickerView.h"
#include "elastos/droid/widget/YearPickerView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CYearPickerView)
    , public YearPickerView
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CYEARPICKERVIEW_H__