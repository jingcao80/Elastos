#ifndef __ELASTOS_DROID_WIDGET_CNUMBERPICKERCUSTOMEDITTEXT_H__
#define __ELASTOS_DROID_WIDGET_CNUMBERPICKERCUSTOMEDITTEXT_H__

#include "_Elastos_Droid_Widget_CNumberPickerCustomEditText.h"
#include "elastos/droid/widget/NumberPicker.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CNumberPickerCustomEditText)
    , public NumberPicker::CustomEditText
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CNUMBERPICKERCUSTOMEDITTEXT_H__
