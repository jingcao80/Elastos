#ifndef __ELASTOS_DROID_DIALER_WIDGET_COVERLAPPINGPAELAYOUTSAVEDSTATE_H__
#define __ELASTOS_DROID_DIALER_WIDGET_COVERLAPPINGPAELAYOUTSAVEDSTATE_H__

#include "_Elastos_Droid_Dialer_Widget_COverlappingPaneLayoutSavedState.h"
#include "elastos/droid/dialer/widget/OverlappingPaneLayout.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Widget {

CarClass(COverlappingPaneLayoutSavedState)
    , public OverlappingPaneLayout::SavedState
{
public:
    CAR_OBJECT_DECL();
};

} // Widget
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_WIDGET_COVERLAPPINGPAELAYOUTSAVEDSTATE_H__
