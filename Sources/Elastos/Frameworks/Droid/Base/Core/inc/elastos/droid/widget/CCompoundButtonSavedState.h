#ifndef __ELASTOS_DROID_WIDGET_CCOMPOUNDBUTTONSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CCOMPOUNDBUTTONSAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CCompoundButtonSavedState.h"
#include "elastos/droid/widget/CompoundButton.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CCompoundButtonSavedState)
    , public CompoundButton::SavedState
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_CCOMPOUNDBUTTONSAVEDSTATE_H__
