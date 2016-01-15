#ifndef  __ELASTOS_DROID_WIDGET_CSPINNERSAVEDSTATE_H__
#define  __ELASTOS_DROID_WIDGET_CSPINNERSAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CSpinnerSavedState.h"
#include "elastos/droid/widget/Spinner.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CSpinnerSavedState) , public Spinner::SavedState
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CSPINNERSAVEDSTATE_H__
