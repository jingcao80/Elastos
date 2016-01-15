#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEWSAVEDSTATE_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEWSAVEDSTATE_H__

#include "_Elastos_Droid_Internal_Widget_CLockPatternViewSavedState.h"
#include "elastos/droid/internal/widget/LockPatternView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CLockPatternViewSavedState), public LockPatternView::SavedState
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEWSAVEDSTATE_H__
