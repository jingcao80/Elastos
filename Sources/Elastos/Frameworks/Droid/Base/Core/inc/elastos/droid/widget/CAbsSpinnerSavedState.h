#ifndef __ELASTOS_DROID_WIDGET_CABSSPINNERSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CABSSPINNERSAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CAbsSpinnerSavedState.h"
#include "elastos/droid/widget/AbsSpinner.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CAbsSpinnerSavedState) , public AbsSpinner::SavedState
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CABSSPINNERSAVEDSTATE_H__
