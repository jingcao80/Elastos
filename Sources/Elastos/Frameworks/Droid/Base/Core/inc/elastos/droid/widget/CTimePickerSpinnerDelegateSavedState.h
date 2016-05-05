#ifndef __ELASTOS_DROID_WIDGET_CTIMEPICKERSPINNERDELEGATESAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CTIMEPICKERSPINNERDELEGATESAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CTimePickerSpinnerDelegateSavedState.h"
#include "elastos/droid/widget/TimePickerSpinnerDelegate.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CTimePickerSpinnerDelegateSavedState)
    , public TimePickerSpinnerDelegate::SavedState
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_CTIMEPICKERSPINNERDELEGATESAVEDSTATE_H__
