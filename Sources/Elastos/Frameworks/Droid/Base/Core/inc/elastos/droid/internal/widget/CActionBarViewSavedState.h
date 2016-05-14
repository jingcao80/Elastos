#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBARVIEWSAVEDSTATE_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBARVIEWSAVEDSTATE_H__

#include "_Elastos_Droid_Internal_Widget_CActionBarViewSavedState.h"
#include "elastos/droid/internal/widget/ActionBarView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CActionBarViewSavedState)
    , public ActionBarView::ActionBarViewSavedState
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBARVIEWSAVEDSTATE_H__
