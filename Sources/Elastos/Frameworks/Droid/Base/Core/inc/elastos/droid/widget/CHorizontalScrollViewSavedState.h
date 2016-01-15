
#ifndef __ELASTOS_DROID_VIEW_CHORIZONTALCSROLLVIEWBASESAVEDSTATE_H__
#define __ELASTOS_DROID_VIEW_CHORIZONTALCSROLLVIEWBASESAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CHorizontalScrollViewSavedState.h"
#include "elastos/droid/widget/HorizontalScrollView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CHorizontalScrollViewSavedState)
    , public HorizontalScrollView::SavedState
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CHORIZONTALCSROLLVIEWBASESAVEDSTATE_H__
